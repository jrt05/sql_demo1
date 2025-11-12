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

/*  #define _GNU_SOURCE */
int main() {
    int rc = 0;
    struct stat file_stat = {0};

    rc = stat("/home/", &file_stat);              /* File stat information */

    printf("Inode numbr:       %u\n"
           "Total Size:        %u\n"
           "blksize:           %u\n"
           "Num blks:          %u\n"
           "Last Access:       %u.%u\n"
           "Last Modification: %u.%u\n"
           "Last status chg:   %u.%u\n",
           (uint32_t)file_stat.st_ino,
           (uint32_t)file_stat.st_size,
           (uint32_t)file_stat.st_blksize,
           (uint32_t)file_stat.st_blocks,
           (uint32_t)file_stat.st_atim.tv_sec, (uint32_t)file_stat.st_atim.tv_nsec,
           (uint32_t)file_stat.st_mtim.tv_sec, (uint32_t)file_stat.st_mtim.tv_nsec,
           (uint32_t)file_stat.st_ctim.tv_sec, (uint32_t)file_stat.st_ctim.tv_nsec);

    return rc;
}
