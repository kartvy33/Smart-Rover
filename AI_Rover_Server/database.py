import os
import sqlite3
from contextlib import contextmanager

DB_PATH = os.getenv("DATABASE_PATH", "data/rover.db")
MAX_ACCOUNTS_PER_EMAIL = 5

def init_db():
    os.makedirs(os.path.dirname(DB_PATH) or ".", exist_ok=True)
    with sqlite3.connect(DB_PATH) as db:
        table = db.execute("SELECT sql FROM sqlite_master WHERE type='table' AND name='users'").fetchone()
        if table and "email TEXT NOT NULL UNIQUE" in (table[0] or ""):
            db.execute("ALTER TABLE users RENAME TO users_old")
            db.execute("""CREATE TABLE users (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL,
                email TEXT NOT NULL,
                password_hash TEXT NOT NULL,
                role TEXT NOT NULL DEFAULT 'user',
                email_verified INTEGER NOT NULL DEFAULT 0,
                verification_token TEXT,
                created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
            )""")
            db.execute("""INSERT INTO users
                (id,name,email,password_hash,role,email_verified,verification_token,created_at)
                SELECT id,name,email,password_hash,role,email_verified,verification_token,created_at
                FROM users_old""")
            db.execute("DROP TABLE users_old")
        else:
            db.execute("""CREATE TABLE IF NOT EXISTS users (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL,
                email TEXT NOT NULL,
                password_hash TEXT NOT NULL,
                role TEXT NOT NULL DEFAULT 'user',
                email_verified INTEGER NOT NULL DEFAULT 0,
                verification_token TEXT,
                created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
            )""")
        db.execute("""CREATE TABLE IF NOT EXISTS plate_events (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            plate TEXT,
            confidence REAL,
            image_path TEXT,
            source TEXT,
            created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
        )""")
        db.commit()

@contextmanager
def connect():
    db = sqlite3.connect(DB_PATH)
    db.row_factory = sqlite3.Row
    try:
        yield db
    finally:
        db.close()

def account_count_for_email(email):
    with connect() as db:
        return db.execute("SELECT COUNT(*) FROM users WHERE email=?", (email.lower(),)).fetchone()[0]

def create_user(name, email, password_hash, role, token):
    with connect() as db:
        cur = db.execute("INSERT INTO users(name,email,password_hash,role,verification_token) VALUES(?,?,?,?,?)",
                         (name, email, password_hash, role, token))
        db.commit()
        return cur.lastrowid

def get_users_by_email(email):
    with connect() as db:
        return db.execute("SELECT * FROM users WHERE email=? ORDER BY id ASC", (email.lower(),)).fetchall()

def get_user_by_email(email):
    users = get_users_by_email(email)
    return users[0] if users else None

def get_user(user_id):
    with connect() as db:
        return db.execute("SELECT * FROM users WHERE id=?", (user_id,)).fetchone()

def verify_user(token):
    with connect() as db:
        row = db.execute("SELECT id FROM users WHERE verification_token=?", (token,)).fetchone()
        if not row:
            return False
        db.execute("UPDATE users SET email_verified=1, verification_token=NULL WHERE id=?", (row["id"],))
        db.commit()
        return True

def add_plate_event(plate, confidence, image_path, source):
    with connect() as db:
        db.execute("INSERT INTO plate_events(plate,confidence,image_path,source) VALUES(?,?,?,?)",
                   (plate, confidence, image_path, source))
        db.commit()

def recent_plate_events(limit=20):
    with connect() as db:
        return [dict(r) for r in db.execute(
            "SELECT * FROM plate_events ORDER BY id DESC LIMIT ?", (limit,)).fetchall()]
