
CC=clang
# -g Debug info
CFLAGS=-Wall -Werror -g -O2
LFLAGS=
#CFLAGS=-Wall -Werror -g -fsanitize=address -fsanitize=undefined
#LFLAGS=-fsanitize=address -fsanitize=undefined

all: pwd.o dbcalls.o main.o sqlite3.o shell.o shell jsrch pwd

pwd: pwd.o dbcalls.o sqlite3.o
	$(CC) $(LFLAGS) -o pwd pwd.o dbcalls.o sqlite3.o

jsrch: main.o dbcalls.o sqlite3.o
	$(CC) $(LFLAGS) -o jsrch main.o dbcalls.o sqlite3.o

shell: shell.o sqlite3.o
	$(CC) $(LFLAGS) -o shell shell.o sqlite3.o

pwd.o: pwd.c sqlite3.h dbcalls.h
	$(CC) -c $(CFLAGS) -o pwd.o pwd.c

shell.o: shell.c sqlite3.h
	$(CC) -c $(CFLAGS) -o shell.o shell.c

main.o: main.c dbcalls.h interface.h main.h sqlite3.h
	$(CC) -c $(CFLAGS) -o main.o main.c

dbcalls.o: dbcalls.c main.h dbcalls.h sqlite3.h
	$(CC) -c $(CFLAGS) -o dbcalls.o dbcalls.c

sqlite3.o: sqlite3.c sqlite3.h
	$(CC) -c $(CFLAGS) -o sqlite3.o sqlite3.c

clean:
	rm *.o jsrch shell pwd
