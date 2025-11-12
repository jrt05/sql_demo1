#ifndef MAIN_H
#define MAIN_H

/* System headers */
#include <stdint.h>

#define bool char
#define true 1
#define false 0

void run_paths(const char* dir, bool changed);
int db_callback(void *NotUsed, int argc, char **argv, char **azColName);
uint32_t stoi(const char *);

#endif // MAIN_H
