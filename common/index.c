/*
 * index.c - 'index.c' module for use in the Tiny Search Engine (TSE) project
 *
 * William Dinauer, Dartmouth CS50 Winter 2021
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hashtable.h"
#include "set.h"
#include "counters.h"
#include "file.h"
#include "string.h"
#include "index.h"

/* ***************** local functions ********************** */
static char *getNextWord(char *line, int *pos, int len);
static void writeSet(void *arg, const char *key, void *item);
static void writeCounters(void *arg, const int key, const int count);

/* ***************** writeIndexToFile ********************** */
/* see index.h for description */
void writeIndexToFile(char *fName, hashtable_t *index)
{
    FILE *fp;
    int len = strlen(fName);
    char *filename = calloc(len + 1, sizeof(char));
    if (filename != NULL) {
        strcpy(filename, fName);
        // try to open a pointer to write to the file
        if ((fp = fopen(filename, "w")) != NULL) {
            // iterate over the hashtable, calling the writeSet function on each set
            hashtable_iterate(index, fp, writeSet);
            fclose(fp);
        }
        free(filename);
    }
}

/* ***************** writeSet ********************** */
/* function to write a set from the inverted index to a line.
 * takes a file pointer as the arg as an indication of the 
 * file to write to
 */
static void writeSet(void *arg, const char *key, void *item)
{
    FILE *fp = arg;
    counters_t *ctrs = item;
    fprintf(fp, "%s", key);         // write the word to a line in the file
    counters_iterate(ctrs, fp, writeCounters);  // iterate over the counter to write all the [docId] [count] pairs on the line
    fputc('\n', fp);                // go to the next line for the next word
}

/* ***************** writeCounters ********************** */
/* writes each (docId, count) pair in the counters on 
 * one line in the file
 */
static void writeCounters(void *arg, const int key, const int count)
{
    FILE *fp = arg;
    fprintf(fp, " %d %d", key, count); // print the [docId] [count] pair for every counter in the counters module
}

/* ***************** loadIndex ********************** */
/* see index.h for description */
hashtable_t *loadIndex(char *fName)
{
    FILE *fp;
    int len = strlen(fName);
    char *filename = calloc(len + 1, sizeof(char));
    if (filename != NULL) {
        strcpy(filename, fName);
        // try to open the index-created file for reading
        if ((fp = fopen(filename, "r")) != NULL) {
            // set the size of the hashtable to the number of lines (# words)
            int size = lines_in_file(fp);
            hashtable_t *index = hashtable_new(size);
            char *line;
            while((line = freadlinep(fp)) != NULL) { // while there is still a line to fetch
                int lineLen = strlen(line);          
                int pos = 0;
                counters_t *ctrs = counters_new();   // create a new counters module for this word
                char *word = getNextWord(line, &pos, lineLen); // get the word associated with the line
                // keep grabbing docId, count pairs and placing them in the counters module
                while (pos < lineLen) {     
                    char *file = getNextWord(line, &pos, lineLen); 
                    char *count = getNextWord(line, &pos, lineLen);
                    // convert strings to ints
                    int fileNum = atoi(file);                       
                    int countNum = atoi(count);
                    counters_set(ctrs, fileNum, countNum);
                    free(file);
                    free(count);
                }
                hashtable_insert(index, word, ctrs); // add the word as the key, the counters as the item
                free(line);
                free(word);
                line = NULL;
            }
            fclose(fp);
            free(filename);
            return index;  //return the completed index
        }
        free(filename);
    }
    return NULL;
}

/* ***************** getNextWord ********************** */
/* gets the next word in a line from the index-produced file.
 * the function starts searching at a given position (pos) and
 * stops once it reaches the end of the line or hits a space, indicating
 * it should return the word from the initial position to its new location
 *
 * The function is inspired by the webpage_getNextWord function
 * for the 'webpage' module
 */
static char *getNextWord(char *line, int *pos, int len)
{
    char* beg = &(line[*pos]);              // a pointer to the initial position
    // increment the position until the end of the line has been reached, or pos points to a space
    while(*pos < len && (line[*pos] != '\0' || line[*pos] != ' ')) {
        (*pos)++;
    }
    char* end = &(line[*pos-1]);            // pointer to the last char in the word
    int wordlen = end - beg + 1;            
    char *word = calloc(wordlen + 1, sizeof(char));
    if (word == NULL) {
        return NULL;
    } else {
        strncpy(word, beg, wordlen);
        return word;
    }
}
