#ifndef DBCALLS_H
#define DBCALLS_H

#include "sqlite3.h"

/* Open the database */
int db_open(sqlite3 **db, const char * db_name);
/* Close the database */
int db_close(sqlite3 **db);
/* Execute SQL */
int db_exec(sqlite3 *db, const char *e, int verbose);

#endif /* DBCALLS_H */
