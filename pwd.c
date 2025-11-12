//#include <iostream>
//#include <string>
#include <string.h>
#include "sqlite3.h"

#include <termios.h>
#include <unistd.h>
#include <stdio.h>

#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdint.h>
#include <limits.h>
#include <assert.h>

uint32_t val_sec;
uint32_t val_nano;

#include "dbcalls.h"

char username[1024];
char password[1024];

sqlite3 *db;
#define FILENAME "./user.db"
char exec[14096];       /* Used to issue SQLite calls */
int commit_count = 0;

unsigned long long hash(const char *str)
{
    unsigned long long hash = 5381;
    int c;

    while ((c = *(str++))) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
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


//using namespace std;
//
int db_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for(i=0; i<argc; i++){
        //printf("%s - %s\n", azColName[i], argv[i]);
        printf("%s: %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    /*printf("\n");*/
    return 0;
}


int getch() {
    int ch;
    struct termios t_old, t_new;

    tcgetattr(STDIN_FILENO, &t_old);
    t_new = t_old;
    t_new.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t_new);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &t_old);
    return ch;
}

char * getuser(const char* prompt) {
    printf("%s", prompt);
    //string username;
    unsigned char ch = 0;
    unsigned int x = 0;
    while((ch = getchar()) != '\n') {
        username[x] = ch;
        ++x;
    }
    return username;
}

char * getpasswd(const char* prompt, int show_asterisk) {
    //const char BACKSPACE = 8;
    //const char RETURN = 13;
    //string password;
    unsigned char ch = 0;
    printf("%s", prompt);
    while ((ch = getch()) != '\n') {
        int pwdlen = strlen(password);
        if (ch == '\b' || ch == 127) {  // Backspace or delete
            if (pwdlen != 0) {
                if (show_asterisk) printf("\b \b");
                password[pwdlen-1] = '\0';
            }
        } else if (ch != 27) { // ignore escape key
            password[pwdlen] = ch;
            password[pwdlen + 1] = '\0';
            if (show_asterisk) printf("*");
        }
    }
    printf("\n");
    return password;
}

int main() {
    const char* promptUser = "Enter User Name: ";
    const char* promptPass = "Enter Password: ";
    getuser(promptUser);
    getpasswd(promptPass, 1);
    //printf("%s\n", getuser(promptUser));
    //printf("%s\n", getpasswd(promptPass, 1));
    int rc = 0;
    db = NULL;

    unsigned long long passhash = hash(password);

    rc = db_open(&db, FILENAME);
    /*
    rc = db_exec(db, "CREATE TABLE IF NOT EXISTS dir_entry "
                     "(name TEXT PRIMARY KEY UNIQUE, second INTEGER, nanosecond INTEGER)");
    */
    rc = db_exec(db, "CREATE TABLE IF NOT EXISTS user_entry "
                     "(name TEXT, password TEXT, phone TEXT, PRIMARY KEY('name'))", 0);

    rc = db_exec(db, "BEGIN TRANSACTION", 0);
    sprintf(exec, "SELECT * FROM user_entry WHERE name = \"%s\" AND password = \"%llu\"", username, passhash);
    rc = db_exec(db, exec, 1);
    /*run_paths("../");*/
    rc = db_exec(db, "COMMIT TRANSACTION", 0);

    rc = db_close(&db);



    return rc;
}





