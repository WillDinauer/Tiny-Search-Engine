# Makefile for 'crawler'
# William Dinauer, Dartmouth CS50 Winter 2021

L = ../libcs50

PROG = crawler
OBJS = crawler.o $L/hashtable.o $L/webpage.o $L/set.o $L/counters.o $L/bag.o $L/memory.o  $L/jhash.o $L/file.o ../common/pagedir.o
LIBS = -lm

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50 -I../common
CC = gcc

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $(PROG)

crawler.o: $L/hashtable.h $L/set.h $L/counters.h $L/bag.h
hashtable.o: $L/hashtable.h $L/set.h $L/memory.h $L/jhash.h 
webpage.o: $L/webpage.h $L/file.h
set.o: $L/set.h $L/memory.h
bag.o: $L/bag.h
counters.o: $L/counters.h $L/memory.h
jhash.o: $L/jhash.h
memory.o: $L/memory.h
file.o: $L/file.h
pagedir.o: ../common/pagedir.h $L/webpage.h

.PHONY: clean valgrind test

test: $(PROG)
	bash -v testing.sh

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f $(PROG)
	rm -f ../libcs50/*.o
	rm -f ../common/*.o
