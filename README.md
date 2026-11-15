# SQL Injection: Authentication Bypass Demo (C++ / MySQL)

A lightweight C++17 web application demonstrating SQL Injection authentication bypass and parameterized query mitigation.

## Setup & Run

### 1. Build
```bash
make
```

### 2. Run
```bash
export DB_HOST=127.0.0.1
export DB_PORT=3306
export DB_USER=root
export DB_PASSWORD="YOUR_PASSWORD"

./sql_injection_lab
```

Open: http://127.0.0.1:8080

## Demonstration Payloads

Enter into the **Username** field with any password:
- `' OR '1'='1' #`
- `admin' #`
- `' OR '1'='1' -- `

Default seeded user: `admin` / `admin123`
