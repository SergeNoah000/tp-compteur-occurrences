CC = gcc
CFLAGS = -Wall -Wextra --no-warning

.PHONY: all compil run c

all:runnerc




#compil:programme

runnerc: fonction.c structure.h tab_processus_threads.c
	$(CC) $(CFLAGS) -o runner structure.h fonction.c tab_processus_threads.c -pthread 

./fonction.o: fonction.c structure.h
	$(CC) $(CFLAGS) -o fonction.o -c fonction.c -I.

./tab_processus_threads.o : tab_processus_threads.c structure.h
	$(CC) $(CFLAGS) -o tab_processus_threads.o -c tab_processus_threads.c -I. -pthread

clean:
	rm -f programme *.txt *.o

run:
	./runner
