# Smart Rover Option D sign-in setup

The server now contains password/email sign-in, email and optional SMS one-time-code verification, Google OAuth, Facebook OAuth, Telegram Login Widget verification, and an operator-only security-events page.

## 1. Install dependencies

From the `AI_Rover_Server` directory:

```bat
python -m pip install -r requirements.txt
```

## 2. Configure the private .env file

Copy `.env.example` to `.env`. Keep `.env` private and never commit it.

Keep the existing `SECRET_KEY`, `TELEGRAM_BOT_TOKEN`, and `TELEGRAM_CHAT_ID` values in that private file. Add provider settings as needed.

### Email verification

For development, `EMAIL_MODE=console` prints the email verification link and OTP in the server terminal. For real email delivery, set `EMAIL_MODE=smtp` and fill in `SMTP_HOST`, `SMTP_PORT`, `SMTP_USERNAME`, `SMTP_PASSWORD`, and `SMTP_FROM`.

### SMS verification

Create a Twilio account and configure `TWILIO_ACCOUNT_SID`, `TWILIO_AUTH_TOKEN`, and `TWILIO_FROM_NUMBER`. Enter phone numbers in international format, including the country code (for India, +91). SMS verification is optional until these settings are configured.

### Google sign-in

Create a Google OAuth web application. Add this authorized redirect URI for local testing:

`http://127.0.0.1:5000/auth/google/callback`

Set `GOOGLE_CLIENT_ID` and `GOOGLE_CLIENT_SECRET` in the private .env file. For a deployed site, register its exact HTTPS callback URL too.

### Facebook sign-in

Create a Facebook app, enable Facebook Login, and add this valid OAuth redirect URI:

`http://127.0.0.1:5000/auth/facebook/callback`

Set `FACEBOOK_CLIENT_ID` and `FACEBOOK_CLIENT_SECRET` in .env. Facebook may require app review or test-user access depending on the app mode and requested permissions.

### Telegram sign-in

Use the same Telegram bot configured for notifications. Set `TELEGRAM_BOT_USERNAME` to the bot username without the @ symbol, and keep `TELEGRAM_BOT_TOKEN` in .env. Configure the website domain for the Telegram Login Widget with BotFather. Use HTTPS for a public deployment.

Telegram-only accounts receive a Smart Rover account ID and a placeholder internal email; Telegram does not provide their email address through the login widget.

## 3. Start the server

```bat
python main.py
```

Open `http://127.0.0.1:5000/login`.

## 4. Operator activity page

After signing in with an operator account, open:

`http://127.0.0.1:5000/admin/security`

The page shows signup, login, and verification events. It records security-related events only, not keystrokes or private messages.

## Important notes

- The existing operator enrollment code is not a substitute for securing the private .env file. Change the default enrollment code before exposing the website.
- Use HTTPS for any public deployment. Do not expose the Flask development server directly to the internet.
- Do not send passwords or OTP codes to Telegram.
- Instagram personal accounts are not supported as general website OAuth login providers.
- This feature set has been committed but still needs local configuration and end-to-end testing with real provider credentials. Hardware rover control is unchanged.
