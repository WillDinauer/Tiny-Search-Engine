# Makefile for 'querier' module
# William Dinauer, Dartmouth CS50 Winter 2021

L = ../libcs50
C = ../common

PROG = querier
LIBS = -lm
LLIBS = $L/libcs50.a
CLIBS = $C/common.a

OBJS = querier.o

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L -I$C
CC = gcc

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $^ $(CLIBS) $(LLIBS) $(LIBS) -o $(PROG)

fuzzquery: fuzzquery.o
	$(CC) $(CFLAGS) $^ $(CLIBS) $(LLIBS) $(LIBS) -o fuzzquery

querier.o: $L/hashtable.h $L/set.h $L/counters.h $L/file.h $C/index.h $C/pagedir.h $C/word.h
fuzzquery.o: $L/memory.h $L/file.h

.PHONY: clean valgrind test

test: $(PROG) fuzzquery
	bash -v testing.sh

valgrind: $(PROG)
	valgrind --leak-check=full ./querier ../data/toscrape-depth-2 ../data/toscrape-index-2

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f $(PROG)
	rm -f fuzzquery
