/*
 * indextest.c - test module for 'indexer' which reads from the file created by the indexer,
 *      recreates the inverted index, and writes the data to a new file
 *
 * William Dinauer, Dartmouth CS50 Winter 2021
 */

#include <stdlib.h>
#include <stdio.h>
#include "file.h"
#include "hashtable.h"
#include "set.h"
#include "counters.h"
#include "index.h"

/* ***************** local functions ********************** */
static void ctrs_delete(void *ctrs);

/* ***************** main ********************** */
int main (int argc, char *argv[])
{
    // simple argument validation - only checking argument count
    if (argc != 3) {
        fprintf(stderr, "two arguments expected (%d given). Usage: ./indextest oldIndexFilename newIndexFilename\n", argc-1);
        return 1;
    }

    hashtable_t *index = loadIndex(argv[1]); // load the inverted index from the indexer-created file (detailed in index.h)
    if (index != NULL) {
        writeIndexToFile(argv[2], index);    // write the index to the new file
        hashtable_delete(index, ctrs_delete);
    } else {  // out of memory or ran into another error while creating the index
        fprintf(stderr, "error creating the index\n");
        return 1;
    }

    return 0;
}

/* ***************** ctrs_delete ********************** */
/* function with a void pointer for deleting the counters
 * in the inverted index
 */
static void ctrs_delete(void *ctrs)
{
    counters_t *counters = ctrs;
    if (counters != NULL) {
        counters_delete(counters);
    }
}
