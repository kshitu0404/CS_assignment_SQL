#include "auth.h"
#include <cstring>
#include <iostream>

AuthResult authenticateVulnerable(MYSQL* conn, const std::string& username, const std::string& password) {
    if (!conn) {
        return {false, "", "No database connection."};
    }

    std::string query = "SELECT id, username FROM users WHERE username = '" + username + "' AND password = '" + password + "';";

    if (mysql_query(conn, query.c_str()) != 0) {
        return {false, "", mysql_error(conn)};
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) {
        return {false, "", mysql_error(conn)};
    }

    my_ulonglong rowCount = mysql_num_rows(res);
    if (rowCount > 0) {
        MYSQL_ROW row = mysql_fetch_row(res);
        std::string matchedUser = (row && row[1]) ? row[1] : "unknown";
        mysql_free_result(res);
        return {true, matchedUser, "Authentication successful. Logged in as " + matchedUser + "."};
    }

    mysql_free_result(res);
    return {false, "", "Authentication failed. Invalid username or password."};
}

AuthResult authenticateSecure(MYSQL* conn, const std::string& username, const std::string& password) {
    if (!conn) {
        return {false, "", "No database connection."};
    }

    const char* query = "SELECT id, username FROM users WHERE username = ? AND password = ?";
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        return {false, "", "Failed to initialize statement."};
    }

    if (mysql_stmt_prepare(stmt, query, std::strlen(query)) != 0) {
        std::string err = mysql_stmt_error(stmt);
        mysql_stmt_close(stmt);
        return {false, "", err};
    }

    MYSQL_BIND bindParams[2];
    std::memset(bindParams, 0, sizeof(bindParams));

    unsigned long userLen = username.length();
    bindParams[0].buffer_type = MYSQL_TYPE_STRING;
    bindParams[0].buffer = (void*)username.c_str();
    bindParams[0].buffer_length = userLen;
    bindParams[0].length = &userLen;

    unsigned long passLen = password.length();
    bindParams[1].buffer_type = MYSQL_TYPE_STRING;
    bindParams[1].buffer = (void*)password.c_str();
    bindParams[1].buffer_length = passLen;
    bindParams[1].length = &passLen;

    if (mysql_stmt_bind_param(stmt, bindParams) != 0 || mysql_stmt_execute(stmt) != 0) {
        std::string err = mysql_stmt_error(stmt);
        mysql_stmt_close(stmt);
        return {false, "", err};
    }

    mysql_stmt_store_result(stmt);
    my_ulonglong rowCount = mysql_stmt_num_rows(stmt);

    if (rowCount > 0) {
        int id = 0;
        char userBuffer[51];
        unsigned long outLengths[2];
        bool isNull[2] = {false, false};

        MYSQL_BIND resultBind[2];
        std::memset(resultBind, 0, sizeof(resultBind));

        resultBind[0].buffer_type = MYSQL_TYPE_LONG;
        resultBind[0].buffer = (char*)&id;

        resultBind[1].buffer_type = MYSQL_TYPE_STRING;
        resultBind[1].buffer = userBuffer;
        resultBind[1].buffer_length = sizeof(userBuffer) - 1;
        resultBind[1].length = &outLengths[1];
        resultBind[1].is_null = &isNull[1];

        mysql_stmt_bind_result(stmt, resultBind);
        mysql_stmt_fetch(stmt);

        userBuffer[outLengths[1]] = '\0';
        std::string matchedUser(userBuffer);

        mysql_stmt_close(stmt);
        return {true, matchedUser, "Authentication successful. Logged in as " + matchedUser + "."};
    }

    mysql_stmt_close(stmt);
    return {false, "", "Authentication failed. Invalid username or password."};
}

