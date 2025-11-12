/*
 *  Author: Jason Torola
 *  Purpose: TBD
 *
 * */

/* System headers */
#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdint.h>
#include <limits.h>
#include <assert.h>

/* User headers */
#include "main.h"
#include "dbcalls.h"
/*#include "interface.h" */     /* Array interfaces */ 

sqlite3 *db;
#define FILENAME "./jsrch.db"
char exec[14096];       /* Used to issue SQLite calls */
int commit_count = 0;

uint32_t val_sec;
uint32_t val_nano;

/*  #define _GNU_SOURCE */
int main() {
    int rc = 0; /* Return code to check database calls */
    db = NULL;

    rc = db_open(&db, FILENAME);
    /*
    rc = db_exec(db, "CREATE TABLE IF NOT EXISTS dir_entry "
                     "(name TEXT PRIMARY KEY UNIQUE, second INTEGER, nanosecond INTEGER)");
    */
    rc = db_exec(db, "CREATE TABLE IF NOT EXISTS dir_entry "
                     "(name TEXT, second TEXT, nanosecond TEXT, PRIMARY KEY('name'))", 0);

    rc = db_exec(db, "BEGIN TRANSACTION", 0);
    //run_paths("/", true);
    /*run_paths("../");*/
    rc = db_exec(db, "COMMIT TRANSACTION", 0);

    rc = db_close(&db);

    return rc;
}



/* We will check all directories. If the current directory
 * is marked as changed, we'll check all the files. If our
 * current directory is not changed, only check the 
 * directories. */
void run_paths(const char* dir, bool changed) {
    struct dirent *dir_entry = NULL;
    DIR *root = NULL;
    char *fulldir = NULL;
    unsigned int dirlen = strlen(dir);      /* Offset to the end of the root directory */
    int rc;
    /* int commit_count = 0; */

    fulldir = (char*)malloc(dirlen+NAME_MAX+10);

    strncpy(fulldir, dir, dirlen+1);   /* Back up the root directory */

    root = opendir(dir);
    /*
    rc = db_exec(db, "COMMIT TRANSACTION");
    rc = db_exec(db, "BEGIN TRANSACTION");
    sprintf(exec, "INSERT INTO dir_entry VALUES(\"%s\")", line);
    rc = db_exec(db, exec);
    rc = db_exec(db, "COMMIT TRANSACTION");
    */

    if(root == NULL) {
        /*printf("Error opening directory: %s\n", dir);*/
        if(fulldir != NULL) {
            free(fulldir);
            fulldir = NULL;
        }
        return;
    }
    else {
        /* printf("Opening %s\n", dir); */
    }

    while((dir_entry = readdir(root))) {
        /* Don't process ./ or ../ */
        if(strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0) {
            continue;
        }

        if(changed && dir_entry->d_type != DT_DIR) {
            continue;
        }

        struct stat file_stat = {0};
        bool curr_dir_changed = false;
        /* Create a full path to the file */
        fulldir[dirlen] = '\0';                  /* Make string only the root directory */
        strcat(fulldir, dir_entry->d_name);      /* Concatenate new file with root */

        /* explain query plan will show what exactly the query does */
        /* Slow, uses SCAN */
        /*sprintf(exec, "explain query plan SELECT name FROM dir_entry WHERE name LIKE \"%s\"", fulldir);*/
        /* Fast, uses SEARCH */
        /*sprintf(exec, "explain query plan SELECT name FROM dir_entry WHERE name = \"%s\"", fulldir);*/

        /* Initialize values to SNO values */
        val_sec = -1;
        val_nano = -1;
        sprintf(exec, "SELECT * FROM dir_entry WHERE name = \"%s\"", fulldir);
        rc = db_exec(db, exec, 0);

        rc = stat(fulldir, &file_stat);              /* File stat information */

        /*bool match = false;*/

        /* If our current directory has changed, we want to check everything.
         * If our current directory has not changed, just check child directories. */
        if(rc == 0 && (dir_entry->d_type == DT_DIR || changed)) {
            if(val_sec != file_stat.st_mtim.tv_sec && val_nano != file_stat.st_mtim.tv_nsec) {
                curr_dir_changed = true;
                sprintf(exec, "INSERT OR REPLACE INTO dir_entry VALUES(\"%s\", %u, %u)", fulldir,
                        (uint32_t)file_stat.st_mtim.tv_sec, (uint32_t)file_stat.st_mtim.tv_nsec);
                printf("%s changed\n", fulldir);
                rc = db_exec(db, exec, 0);
                if(++commit_count > 10000) {
                    printf("Writing to disk\n");
                    rc = db_exec(db, "COMMIT TRANSACTION", 0);
                    rc = db_exec(db, "BEGIN TRANSACTION", 0);
                    commit_count = 0;
                }
            }
            else {
                /*match = true; */
            }
        }
        else {
            /* printf("Error: Couldn't stat %s\n", fulldir); */
        }

        /* If current entry is a directory, then iterate through it */
        /* if(!match && dir_entry->d_type == DT_DIR) { */
        if(dir_entry->d_type == DT_DIR) {
            strcat(fulldir, "/");
            run_paths(fulldir, curr_dir_changed);
        }
    }

    /* Clean up */
    if(root != NULL) {
        closedir(root);
        root = NULL;
    }
    if(fulldir != NULL) {
        free(fulldir);
        fulldir = NULL;
    }

    return;
}

int db_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for(i=0; i<argc; i++){
        if(strcmp(azColName[i], "second") == 0) {
            val_sec = stoi(argv[i]);
        }
        else if(strcmp(azColName[i], "nanosecond") == 0) {
            val_nano = stoi(argv[i]);
        }
        /*printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");*/
    }
    /*printf("\n");*/
    return 0;
}

/* Convert a string to an integer */
uint32_t stoi(const char *s) {
    long long ret = 0;
    int len = strlen(s);
    if(len > 10) {
        printf("%s\n", s);
    }
    assert(len <= 10);
    if(len > 10) return 0;

    const char *t = s;
    /* Convert string to int */
    while(*t != '\0') {
        ret = (ret * 10) + (*t - '0');
        ++t;
    }

    assert(ret <= UINT32_MAX);
    if(ret > UINT32_MAX) ret = 0;

    return (uint32_t)ret;
}





