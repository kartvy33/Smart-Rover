"""Identity verification helpers for Smart Rover.

This module supports short-lived email/SMS one-time codes and a minimal
security audit trail. Provider credentials must be kept in the local .env.
"""
import hashlib
import hmac
import os
import secrets
import sqlite3
import time
from contextlib import contextmanager
from email.message import EmailMessage
import smtplib

import requests

DB_PATH = os.getenv("DATABASE_PATH", "data/rover.db")
OTP_TTL_SECONDS = int(os.getenv("OTP_TTL_SECONDS", "600"))
OTP_MAX_ATTEMPTS = int(os.getenv("OTP_MAX_ATTEMPTS", "5"))


def init_identity_tables():
    os.makedirs(os.path.dirname(DB_PATH) or ".", exist_ok=True)
    with sqlite3.connect(DB_PATH) as db:
        db.execute("""CREATE TABLE IF NOT EXISTS identity_otps (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            channel TEXT NOT NULL,
            destination TEXT NOT NULL,
            code_hash TEXT NOT NULL,
            expires_at INTEGER NOT NULL,
            attempts INTEGER NOT NULL DEFAULT 0,
            consumed INTEGER NOT NULL DEFAULT 0,
            created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
        )""")
        db.execute("""CREATE TABLE IF NOT EXISTS security_events (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER,
            event TEXT NOT NULL,
            method TEXT,
            ip_address TEXT,
            user_agent TEXT,
            created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
        )""")
        db.commit()


@contextmanager
def _connect():
    db = sqlite3.connect(DB_PATH)
    db.row_factory = sqlite3.Row
    try:
        yield db
    finally:
        db.close()


def _code_digest(code, salt):
    return hashlib.sha256((salt + code).encode("utf-8")).hexdigest()


def create_otp(user_id, channel, destination):
    if channel not in ("email", "sms"):
        raise ValueError("Unsupported verification channel.")
    code = f"{secrets.randbelow(1000000):06d}"
    salt = secrets.token_hex(16)
    digest = _code_digest(code, salt)
    with _connect() as db:
        db.execute(
            "UPDATE identity_otps SET consumed=1 WHERE user_id=? AND channel=? AND consumed=0",
            (user_id, channel),
        )
        db.execute(
            """INSERT INTO identity_otps
               (user_id,channel,destination,code_hash,expires_at)
               VALUES(?,?,?,?,?)""",
            (user_id, channel, destination, salt + ":" + digest, int(time.time()) + OTP_TTL_SECONDS),
        )
        db.commit()
    if channel == "email":
        _send_email_code(destination, code)
    else:
        _send_sms_code(destination, code)
    return True


def verify_otp(user_id, channel, code):
    if not isinstance(code, str) or len(code) != 6 or not code.isdigit():
        return False
    with _connect() as db:
        row = db.execute(
            """SELECT * FROM identity_otps
               WHERE user_id=? AND channel=? AND consumed=0
               ORDER BY id DESC LIMIT 1""",
            (user_id, channel),
        ).fetchone()
        if not row or row["expires_at"] < int(time.time()) or row["attempts"] >= OTP_MAX_ATTEMPTS:
            return False
        salt, stored = row["code_hash"].split(":", 1)
        valid = hmac.compare_digest(_code_digest(code, salt), stored)
        db.execute("UPDATE identity_otps SET attempts=attempts+1, consumed=? WHERE id=?",
                   (1 if valid else 0, row["id"]))
        db.commit()
        return valid


def _send_email_code(destination, code):
    mode = os.getenv("EMAIL_MODE", "console").lower()
    if mode == "console":
        print(f"Smart Rover email verification code for {destination}: {code}")
        return
    message = EmailMessage()
    message["Subject"] = "Your Smart Rover verification code"
    message["From"] = os.getenv("SMTP_FROM") or os.getenv("SMTP_USERNAME")
    message["To"] = destination
    message.set_content(f"Your Smart Rover verification code is {code}. It expires in 10 minutes.")
    with smtplib.SMTP(os.getenv("SMTP_HOST"), int(os.getenv("SMTP_PORT", "587"))) as smtp:
        smtp.starttls()
        smtp.login(os.getenv("SMTP_USERNAME"), os.getenv("SMTP_PASSWORD"))
        smtp.send_message(message)


def _send_sms_code(destination, code):
    sid = os.getenv("TWILIO_ACCOUNT_SID", "")
    token = os.getenv("TWILIO_AUTH_TOKEN", "")
    sender = os.getenv("TWILIO_FROM_NUMBER", "")
    if not all((sid, token, sender)):
        raise RuntimeError("SMS is not configured. Add Twilio credentials to your private .env file.")
    response = requests.post(
        f"https://api.twilio.com/2010-04-01/Accounts/{sid}/Messages.json",
        data={"From": sender, "To": destination,
              "Body": f"Your Smart Rover verification code is {code}. It expires in 10 minutes."},
        auth=(sid, token),
        timeout=15,
    )
    response.raise_for_status()


def record_security_event(user_id, event, method, ip_address, user_agent):
    with _connect() as db:
        db.execute(
            """INSERT INTO security_events
               (user_id,event,method,ip_address,user_agent) VALUES(?,?,?,?,?)""",
            (user_id, event, method, ip_address, (user_agent or "")[:300]),
        )
        db.commit()


def recent_security_events(limit=100):
    with _connect() as db:
        rows = db.execute(
            """SELECT e.*, u.name, u.email FROM security_events e
               LEFT JOIN users u ON u.id=e.user_id
               ORDER BY e.id DESC LIMIT ?""",
            (max(1, min(int(limit), 200)),),
        ).fetchall()
        return [dict(row) for row in rows]
