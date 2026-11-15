#ifndef AUTH_H
#define AUTH_H

#include <mysql.h>
#include <string>

struct AuthResult {
    bool success;
    std::string username;
    std::string message;
};

AuthResult authenticateVulnerable(MYSQL* conn, const std::string& username, const std::string& password);
AuthResult authenticateSecure(MYSQL* conn, const std::string& username, const std::string& password);

#endif

