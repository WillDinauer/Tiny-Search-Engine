# Makefile for 'indexer'
# William Dinauer, Dartmouth CS50 Winter 2021

L = ../libcs50
C = ../common

PROG = indexer
LIBS = -lm
LLIBS = $L/libcs50.a
CLIBS = $C/common.a

OBJS = indexer.o

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L -I$C
CC = gcc

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LLIBS) $(CLIBS) $(LIBS) -o $(PROG)

indexer.o: $L/hashtable.h $L/set.h $L/counters.h
indextest.o: $L/file.h $L/hashtable.h $L/set.h $L/counters.h $C/index.h

.PHONY: clean valgrind test

# tests the indexer and indextest modules. Expects a crawler-produced directory ../data
# to exist and be populated with crawled webpages. 
test: $(PROG) indextest
	bash -v testing.sh

# expects a crawler-produced directory ../data to exist and be populated with crawled webpages
# writes the inverted index to the file "index"
valgrind: $(PROG)
	valgrind --leak-check=full ./indexer ../data/ index

# basic test for the indextest
# expects a file "index" to exist and be populated with the index written by the indexer
indextest: indextest.o
	$(CC) $(CFLAGS) $^ $(LLIBS) $(CLIBS) $(LIBS) -o indextest

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f $(PROG)
	rm -f indextest
