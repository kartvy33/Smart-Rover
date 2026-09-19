import hmac
import os
import secrets
import smtplib
from email.message import EmailMessage
from functools import wraps

from flask import jsonify, session
from werkzeug.security import generate_password_hash, check_password_hash
from database import create_user, get_user_by_email, account_count_for_email, MAX_ACCOUNTS_PER_EMAIL, verify_user as db_verify_user

AUTHORIZED_NAME = os.getenv("AUTHORIZED_NAME", "Kartvy").strip().lower()
ENROLLMENT_CODE = os.getenv("AUTHORIZED_ENROLLMENT_CODE", "ROVER-2026")

def hash_password(password):
    return generate_password_hash(password, method="pbkdf2:sha256", salt_length=16)

def create_account(name, email, password, enrollment_code):
    name = (name or "").strip()
    email = (email or "").strip().lower()
    if not name or "@" not in email:
        return None, "Name and valid email are required."
    if not isinstance(password, str) or len(password) < 8:
        return None, "Password must contain at least 8 characters."
    if account_count_for_email(email) >= MAX_ACCOUNTS_PER_EMAIL:
        return None, f"This email has reached the maximum of {MAX_ACCOUNTS_PER_EMAIL} accounts."
    operator = name.lower() == AUTHORIZED_NAME and hmac.compare_digest(enrollment_code or "", ENROLLMENT_CODE)
    role = "operator" if operator else "user"
    token = secrets.token_urlsafe(32)
    user_id = create_user(name, email, hash_password(password), role, token)
    return {"id": user_id, "name": name, "email": email, "role": role, "token": token}, None

def authenticate(email, password):
    user = get_user_by_email((email or "").strip().lower())
    if not user or not check_password_hash(user["password_hash"], password or ""):
        return None, "Invalid email or password."
    if not user["email_verified"]:
        return None, "Email is not verified yet."
    return dict(user), None

def verify_user(token):
    if not token:
        return False
    return db_verify_user(token)

def login_required(fn):
    @wraps(fn)
    def wrapper(*args, **kwargs):
        if not session.get("user_id"):
            return jsonify({"ok": False, "error": "Login required"}), 401
        return fn(*args, **kwargs)
    return wrapper

def operator_required(fn):
    @wraps(fn)
    def wrapper(*args, **kwargs):
        if not session.get("user_id"):
            return jsonify({"ok": False, "error": "Login required"}), 401
        if session.get("role") != "operator":
            return jsonify({"ok": False, "error": "Operator authorization required"}), 403
        return fn(*args, **kwargs)
    return wrapper

def send_verification_email(user, token):
    mode = os.getenv("EMAIL_MODE", "console").lower()
    base = os.getenv("APP_BASE_URL", "http://127.0.0.1:5000")
    link = f"{base}/verify-email?token={token}"
    if mode == "console":
        print("\nEMAIL VERIFICATION LINK:\n" + link + "\n")
        return True
    msg = EmailMessage()
    msg["Subject"] = "Smart Rover email verification"
    msg["From"] = os.getenv("SMTP_FROM") or os.getenv("SMTP_USERNAME")
    msg["To"] = user["email"]
    msg.set_content(f"Verify your Smart Rover account:\n\n{link}\n")
    with smtplib.SMTP(os.getenv("SMTP_HOST"), int(os.getenv("SMTP_PORT", "587"))) as smtp:
        smtp.starttls()
        smtp.login(os.getenv("SMTP_USERNAME"), os.getenv("SMTP_PASSWORD"))
        smtp.send_message(msg)
    return True
