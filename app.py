from contextlib import closing
from pathlib import Path
import sqlite3
from flask import Flask, redirect, render_template, request, url_for

BASE_DIR = Path(__file__).resolve().parent
DATABASE = BASE_DIR / "demo.db"

app = Flask(__name__)

SEED_USERS = (
    ("admin", "admin123"),
    ("alice", "alice-pass"),
    ("bob", "bob-pass"),
)


def init_db():
    with closing(sqlite3.connect(DATABASE)) as db:
        db.execute(
            """
            CREATE TABLE IF NOT EXISTS users (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                username TEXT NOT NULL UNIQUE,
                password TEXT NOT NULL
            )
            """
        )
        for username, password in SEED_USERS:
            db.execute(
                """
                INSERT INTO users (username, password)
                SELECT ?, ?
                WHERE NOT EXISTS (
                    SELECT 1 FROM users WHERE username = ?
                )
                """,
                (username, password, username),
            )
        db.commit()


def make_result(row, query, parameters=None, error=None):
    result = {
        "success": False,
        "matched_username": None,
        "is_admin": False,
        "query": query,
        "parameters": parameters,
        "error": error,
    }
    if row is not None:
        matched_username = row["username"]
        result.update(
            success=True,
            matched_username=matched_username,
            is_admin=matched_username == "admin",
        )
    return result


def vulnerable_login(username, password):
    query = (
        "SELECT id, username, password FROM users "
        f"WHERE username='{username}' AND password='{password}'"
    )
    try:
        with closing(sqlite3.connect(DATABASE)) as db:
            db.row_factory = sqlite3.Row
            row = db.execute(query).fetchone()
    except sqlite3.Error as exc:
        return make_result(None, query, error=str(exc))
    return make_result(row, query)


def fixed_login(username, password):
    query = (
        "SELECT id, username, password FROM users "
        "WHERE username = ? AND password = ?"
    )
    try:
        with closing(sqlite3.connect(DATABASE)) as db:
            db.row_factory = sqlite3.Row
            row = db.execute(query, (username, password)).fetchone()
    except sqlite3.Error as exc:
        return make_result(None, query, error=str(exc))
    return make_result(row, query, parameters=(username, password))


@app.get("/")
def index():
    return redirect(url_for("login"))


@app.route("/login", methods=["GET", "POST"])
def login():
    result = None
    username = ""
    password = ""
    if request.method == "POST":
        username = request.form.get("username", "")
        password = request.form.get("password", "")
        result = vulnerable_login(username, password)

    return render_template(
        "login.html",
        vulnerable_result=result,
        fixed_result=None,
        vulnerable_username=username,
        vulnerable_password=password,
        fixed_username="",
        fixed_password="",
    )


@app.route("/login/fixed", methods=["GET", "POST"])
def login_fixed():
    result = None
    username = ""
    password = ""
    if request.method == "POST":
        username = request.form.get("username", "")
        password = request.form.get("password", "")
        result = fixed_login(username, password)

    return render_template(
        "login.html",
        vulnerable_result=None,
        fixed_result=result,
        vulnerable_username="",
        vulnerable_password="",
        fixed_username=username,
        fixed_password=password,
    )


init_db()

if __name__ == "__main__":
    app.run(host="127.0.0.1", port=5000, debug=False)
