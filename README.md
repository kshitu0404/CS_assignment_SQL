# SQL Injection: Authentication Bypass Demo

A minimal Flask + SQLite application demonstrating SQL Injection authentication bypass and parameterized query mitigation.

## Setup & Run

```bash
pip install -r requirements.txt
python app.py
```

Open: http://127.0.0.1:5000/login

## Test Payloads

Enter any of the following into the **Username** field with any password:

1. `' OR '1'='1' --`
2. `admin' --`
3. `' OR username='admin' --`

Default seeded accounts:
- `admin` / `admin123`
- `alice` / `alice-pass`
- `bob` / `bob-pass`
