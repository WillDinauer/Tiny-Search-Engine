/*
 * index.h - header file for 'index' module used in the Tiny Search Engine (TSE) project 
 * 
 * William Dinauer, Dartmouth CS50 Winter 2021
 */

#ifndef __INDEX_H
#define __INDEX_H

#include <stdlib.h>
#include <stdio.h>
#include "hashtable.h"
#include "set.h"
#include "counters.h"
#include "file.h"
#include "string.h"

/* ***************** writeIndexToFile ********************** */
/* given a filename and the inverted index data structure,
 * write the index into the file. 
 * each line in the file is associated with a single word.
 * lines are written in the form: word docId count [docId] [count]...
 * where docId count pairs are repeated for every pair in counters
 */
void writeIndexToFile(char *fName, hashtable_t *index);

/* ***************** loadIndex ********************** */
/* reads from a file produced by the indexer to create
 * the inverted index data structure (stored in a hashtable).
 * We return: 
 *   a pointer to a hashtable containing the inverted index.
 *   the caller must later free this pointer by calling
 *   hashtable_delete on the index.
 */
hashtable_t *loadIndex(char *fName);

#endif //__INDEX_H
