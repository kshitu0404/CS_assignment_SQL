#ifndef DATABASE_H
#define DATABASE_H

#include <mysql.h>

MYSQL* initDatabase();
void closeDatabase(MYSQL* conn);

#endif

