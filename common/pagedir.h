/*
 * pagedir.h - header file for 'pagedir' used in the "Tiny Search Engine" (TSE) project
 * 
 * William Dinauer, Dartmouth CS50 Winter 2021
 */
#ifndef __PAGEDIR_H
#define __PAGEDIR_H

#include <stdio.h>
#include <stdbool.h>
#include "webpage.h"

/* ***************** checkDirectory ********************** */
/* validates the existence of the data directory to ensure
 * files can be created and written to there
 * We return:
 *  - True if a writtable file can be created and opened in
 *      the directory; hence the directory is valid
 *  - False if the file cannot be opened; invalid directory
 * Note:
 *  the file .crawler is left in the directory upon valid return
 */
bool checkDirectory(char *dir);

/* ***************** isCrawlerDir ********************** */
/* checks that a given directory is crawler produced
 * by trying to open the file './crawler' in the directory
 * for reading.
 * We return:
 *  - true if the ./crawler file is readable within the directory
 *  - false if the file cannot be opened indicating the directory
 *      was not produced by the 'crawler' module
 */
bool isCrawlerDir(char *dir);

/* ***************** checkFile ********************** */
/* checks if the file in the specified directory with the
 * given filename can be opened for the purposes of the openParam.
 * openParam is either "r" or "w" based on whether we want to check
 * if we can read from or write to the file.
 * We return: 
 *   - true if the file in the given directory could be
 *   read to or written to based on the openParam
 *   - false if the file could not be read to or written to,
 *   or if we run out of memory
 */
bool checkFile(char *dir, char *fname, char *openParam);

/* ***************** getNextWebpage ********************** */
/* fetches the webpage with the given filename = docId in the
 * given directory (dir). The function assumes the docId file
 * is in the form:
 * url
 * depth
 * html
 * We return: 
 *      - a pointer to a webpage with the url, depth,
 *      and html parameters as read from the file
 *      - NULL if the file cannot be opened or we run
 *      out of memory creating the file strings or 
 *      the webpage
 */
webpage_t *getNextWebpage(char *dir, int *docId);

/* ***************** savePage ********************** */
/* Writes the words line-by-line from a given 
 * webpage (wp) to a file in the data directory (dir)
 * Names the file based on the fileNum
 */
void savePage(char *dir, webpage_t *wp, int fileNum);

#endif //__PAGEDIR_H
