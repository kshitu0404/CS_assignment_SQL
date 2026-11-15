#include "auth.h"
#include "database.h"
#include "httplib.h"
#include <iostream>
#include <string>

static const size_t MAX_INPUT_LEN = 256;
typedef AuthResult (*AuthFn)(MYSQL*, const std::string&, const std::string&);

static std::string escapeJson(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else out += c;
    }
    return out;
}

static void handleLogin(MYSQL* conn, const httplib::Request& req, httplib::Response& res, AuthFn authFn) {
    if (!req.has_param("username") || !req.has_param("password")) {
        res.status = 400;
        res.set_content("{\"status\":\"error\",\"message\":\"Username and password are required.\"}", "application/json");
        return;
    }

    std::string username = req.get_param_value("username");
    std::string password = req.get_param_value("password");

    if (username.empty() || password.empty() || username.size() > MAX_INPUT_LEN || password.size() > MAX_INPUT_LEN) {
        res.status = 400;
        res.set_content("{\"status\":\"error\",\"message\":\"Inputs must be between 1 and 256 characters.\"}", "application/json");
        return;
    }

    AuthResult r = authFn(conn, username, password);
    if (r.success) {
        res.status = 200;
        res.set_content("{\"status\":\"success\",\"message\":\"" + escapeJson(r.message) + "\"}", "application/json");
    } else {
        res.status = 401;
        res.set_content("{\"status\":\"rejected\",\"message\":\"" + escapeJson(r.message) + "\"}", "application/json");
    }
}

int main() {
    MYSQL* conn = initDatabase();
    if (!conn) {
        std::cerr << "Could not connect to database." << std::endl;
        return 1;
    }

    httplib::Server svr;
    svr.set_mount_point("/", "./public");

    svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.status = 200;
        res.set_content("{\"status\":\"ok\"}", "application/json");
    });

    svr.Post("/vulnerable/login", [conn](const httplib::Request& req, httplib::Response& res) {
        handleLogin(conn, req, res, authenticateVulnerable);
    });

    svr.Post("/secure/login", [conn](const httplib::Request& req, httplib::Response& res) {
        handleLogin(conn, req, res, authenticateSecure);
    });

    std::cout << "Server listening on http://127.0.0.1:8080" << std::endl;
    svr.listen("127.0.0.1", 8080);

    closeDatabase(conn);
    return 0;
}

