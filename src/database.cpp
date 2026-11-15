#include "database.h"
#include <cstdlib>
#include <cstring>
#include <iostream>

static std::string getEnvOrDefault(const char* name, const char* fallback) {
    const char* val = std::getenv(name);
    return (val && val[0] != '\0') ? val : fallback;
}

MYSQL* initDatabase() {
    MYSQL* conn = mysql_init(nullptr);
    if (!conn) {
        std::cerr << "Failed to initialize MySQL structure." << std::endl;
        return nullptr;
    }

    std::string host = getEnvOrDefault("DB_HOST", "127.0.0.1");
    int port = std::atoi(getEnvOrDefault("DB_PORT", "3306").c_str());
    std::string user = getEnvOrDefault("DB_USER", "root");
    std::string password = getEnvOrDefault("DB_PASSWORD", "");

    if (!mysql_real_connect(conn, host.c_str(), user.c_str(), password.c_str(), nullptr, port, nullptr, 0)) {
        FILE* fp = popen("ip route show default 2>/dev/null | awk '{print $3}'", "r");
        char gateway[64] = {0};
        if (fp) {
            if (fgets(gateway, sizeof(gateway) - 1, fp)) {
                char* nl = std::strchr(gateway, '\n');
                if (nl) *nl = '\0';
            }
            pclose(fp);
        }

        bool reconnected = false;
        if (gateway[0] != '\0' && host != gateway) {
            if (mysql_real_connect(conn, gateway, user.c_str(), password.c_str(), nullptr, port, nullptr, 0)) {
                reconnected = true;
            }
        }

        if (!reconnected) {
            std::cerr << "MySQL Connection Error: " << mysql_error(conn) << std::endl;
            mysql_close(conn);
            return nullptr;
        }
    }

    if (mysql_query(conn, "CREATE DATABASE IF NOT EXISTS sql_injection_lab;") != 0 ||
        mysql_select_db(conn, "sql_injection_lab") != 0 ||
        mysql_query(conn, "CREATE TABLE IF NOT EXISTS users (id INT AUTO_INCREMENT PRIMARY KEY, username VARCHAR(50) NOT NULL UNIQUE, password VARCHAR(100) NOT NULL);") != 0 ||
        mysql_query(conn, "INSERT IGNORE INTO users (username, password) VALUES ('admin', 'admin123');") != 0) {
        std::cerr << "Database setup error: " << mysql_error(conn) << std::endl;
        mysql_close(conn);
        return nullptr;
    }

    return conn;
}

void closeDatabase(MYSQL* conn) {
    if (conn) {
        mysql_close(conn);
    }
}

