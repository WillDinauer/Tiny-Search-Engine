# Makefile for 'common'
# William Dinauer, Dartmouth CS50 Winter 2021
L = ../libcs50

OBJS = pagedir.o index.o word.o $L/file.o
LIB = common.a
LLIBS = $L/libcs50.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L
CC = gcc
MAKE = make

$(LIB): $(OBJS)
	ar cr $(LIB) $(LLIBS) $(OBJS)

pagedir.o: pagedir.h $L/webpage.h
index.o: index.h $L/file.h $L/hashtable.h $L/set.h $L/counters.h
word.o: word.h
file.o: $L/file.h

.PHONY: clean

clean:
	rm -f core
	rm -f $(LIB) *~ *.o
	rm -f *.dSYM
	rm -f ../libcs50/file.o
