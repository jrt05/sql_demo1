
/* System headers */
#include <stdio.h>
#include <stdlib.h>

/* User headers */
#include "dbcalls.h"
#include "sqlite3.h"
#include "main.h"

/*
static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}
*/

/* Open the database */
int db_open(sqlite3 **db, const char * db_name) {
    int rc = 0;
    if(*db == NULL) {
        rc = sqlite3_open(db_name, db);
        //printf("sqlite3_open rc: %d\n", rc);
    }
    return rc;
}

/* Close the database */
int db_close(sqlite3 **db) {
    int rc = 0;
    if(*db != NULL) {
        rc = sqlite3_close(*db);
        *db = NULL;
        //printf("sqlite3_close rc: %d\n", rc);
    }
    return rc;
}

/* Execute SQL */
int db_exec(sqlite3 *db, const char *exec_stmnt, int verbose) {
    int rc = 0;
    char *zErrMsg = 0;

    rc = sqlite3_exec(db, exec_stmnt, db_callback, 0, &zErrMsg);

    if(verbose) { 
        printf("sqlite3_exec rc: %d stmnt: %s\n", rc, exec_stmnt);
    }

    if( rc!=SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    return rc;
}

