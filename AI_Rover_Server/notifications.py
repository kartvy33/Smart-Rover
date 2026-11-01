"""Telegram notification helpers. No-op unless bot credentials are configured."""
import os
import requests


def send_telegram_message(message):
    token = os.getenv("TELEGRAM_BOT_TOKEN", "").strip()
    chat_id = os.getenv("TELEGRAM_CHAT_ID", "").strip()
    if not token or not chat_id:
        return False
    try:
        response = requests.post(
            f"https://api.telegram.org/bot{token}/sendMessage",
            data={"chat_id": chat_id, "text": str(message)[:3500]},
            timeout=8,
        )
        response.raise_for_status()
        return True
    except requests.RequestException:
        return False
