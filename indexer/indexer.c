/*
 * indexer.c - 'indexer' module for use in the Tiny Search Engine (TSE) project
 *
 * William Dinauer, Dartmouth CS50 Winter 2021
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "webpage.h"
#include "string.h"
#include "counters.h"
#include "hashtable.h"
#include "set.h"
#include "pagedir.h"
#include "word.h"
#include "index.h"

/* ***************** local functions ********************** */
static bool validateParameters(int argc, char *argv[]); 
static bool createIndex(hashtable_t *index, char *argv[]);
static void ctrs_delete(void *ctrs);

/* ***************** main ********************** */
int main(int argc, char *argv[])
{
    const int INDEX_SIZE = 100;
    if (!validateParameters(argc, argv)) {
       return 1;
    }

    hashtable_t *index = hashtable_new(INDEX_SIZE);
    if (index == NULL) { // out of memory
        fprintf(stderr, "ran out of memory creating index\n");
        return 1;
    } else {
        // create the index and write it to a file
        if (createIndex(index, argv)) {
            writeIndexToFile(argv[2], index);
        } else {
            return false;
        }
        hashtable_delete(index, ctrs_delete);
    }
    return 0;
}

/* ***************** createIndex ********************** */
/* builds the inverted index mapping words to counters,
 * each counter of which holds a docId, count pair 
 * corresponding to the frequency of the word for that webpage
 */ 
static bool createIndex(hashtable_t *index, char* argv[])
{
    int docId = 1;      //docIds are assumed to begin at 1 in the crawler-produced directory
    webpage_t *wp;
    while ((wp = getNextWebpage(argv[1], &docId)) != NULL) { // call the getNextWebpage function, detailed in index.h
        if (wp != NULL) {
            int pos = 0;
            char *word;
            while ((word = webpage_getNextWord(wp, &pos)) != NULL) {
                NormalizeWord(word);                         // detailed in word.h
                // try to create and insert a new counters module into the hashtable
                counters_t *counters = counters_new();
                if (counters != NULL){
                    // on success, add to this newly created counters
                    if (hashtable_insert(index, word, counters)) {
                        counters_add(counters, docId);      
                    // otherwise, a counters already exists for this word. find that counters structure and add to it
                    } else {
                        free(counters);
                        counters_t *ctrs;
                        if ((ctrs = hashtable_find(index, word)) != NULL) {
                            counters_add(ctrs, docId);
                        }
                    }
                } else { // unable to create a new counters: out of memory
                    fprintf(stderr, "error creating module 'counters'; out of memory\n");
                    return false;
                }
                free(word);
                word = NULL;
            }
            webpage_delete(wp);
        } else {
            fprintf(stderr, "error allocating memory for module 'webpage'; out of memory\n");
            return false;
        }
        docId++;        // increment the docId for the next file to be read
    }
    return true;
}

/* ***************** validateParameters ********************** */
/* validates the command-line arguments provided by the user, ensuring
 * usage is in the form: ./indexer pageDirectory indexFilename
 * where pageDirectory is a valid crawler-produced directory and
 * indexFilename is a valid file that the inverted index can be written to
 */
static bool validateParameters(int argc, char *argv[])
{
    // check argument count
    if (argc != 3) {
        fprintf(stderr, "2 arguments expected  (%d received). Usage: ./indexer pageDirectory indexFilename\n", argc-1);
        return false;
    }
    // check that the provided directory exists and is crawler-produced 
    if (!isCrawlerDir(argv[1])) {
        fprintf(stderr, "'%s' is not a valid crawler-produced directory\n", argv[1]);
        return false;
    }
    // check that the provided file can be written to
    if (!checkFile("", argv[2], "w")) {
        fprintf(stderr, "'%s' is not a valid writeable file\n", argv[2]);
        return false;
    }
    return true;
}

/* ***************** ctrs_delete ********************** */
/* function with a pointer to a void ctrs for use
 * when deleting the inverted index
 */
static void ctrs_delete(void *ctrs)
{
    counters_t *counters = ctrs;
    if (counters != NULL) {
        counters_delete(counters);
    }
}
