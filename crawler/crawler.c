/*
 * crawler.c - CS50 'crawler' module for use in the "Tiny Search Engine" (TSE) project
 *
 * William Dinauer, Dartmouth CS50 Winter 2021
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "string.h"
#include "bag.h"
#include "memory.h"
#include "webpage.h"
#include "set.h"
#include "hashtable.h"
#include "pagedir.h"

/* ***************** functions ********************** */
bool crawler(bag_t *pages, hashtable_t *urls, char* argv[], int *maxDepth);
bool validateParameters(int argc, char* argv[], int *depth);
inline static void logr(const char *word, const int depth, const char *url);
void webpageDelete(void *wp);

/* ***************** main ********************** */
int main(const int argc, char* argv[])
{
    const int HTABLE_SIZE = 50;   // size of the hashtable
    int len;                      // length of the url provided by the caller
    int maxDepth;                 // depth to traverse to, provided by caller

    // validate parameters
    if (!validateParameters(argc, argv, &maxDepth)) {
        return 1;
    }

    // create data modules 
    bag_t *pages = bag_new();
    hashtable_t *urls = hashtable_new(HTABLE_SIZE);
    if (pages == NULL || urls == NULL) {    // out of memory
        fprintf(stderr, "error creating data modules\n");
        if (pages != NULL) { 
            free(pages);
        }
        return 1;
    }

    // initialize and allocate memory for the parsed url
    len = strlen(argv[1]);
    char *url = calloc(len + 1, sizeof(char));
    if (url == NULL) {  // out of memory
        fprintf(stderr, "error allocating memory for url");
        return 1;
    }
    strcpy(url, argv[1]);
    
    // create the initial webpage, insert into the bag and hashtable
    webpage_t *pg = webpage_new(url, 0, NULL);
    bag_insert(pages, pg);
    hashtable_insert(urls, url, "");

    // crawl the webpage(s)
    if (!crawler(pages, urls, argv, &maxDepth)) {
        return 1;
    }

    // delete the data structures
    hashtable_delete(urls, NULL); 
    bag_delete(pages, webpageDelete);
    return 0;
}

/* ******************** crawler ************************* */
/* Crawls a webpage, searching for urls and storing html
 * data in the page directory
 * We Return:
 *   - True if the crawl successfully finishes, exploring
 *      all webpages to the maximum depth
 *   - False if we run out of memory, failing to complete
 *      the entire crawl
 */
bool crawler(bag_t *pages, hashtable_t *urls, char* argv[], int *maxDepth) {
    int fileNum = 1;        // tracks the number of files in pageDir for naming purposes
    char *url;
    webpage_t *pg;

    while ((pg = bag_extract(pages)) != NULL) { // while there is a webpage to check...
        if (webpage_fetch(pg)) {                // try to fetch the webpage's html
            int depth = webpage_getDepth(pg);   // depth where this specific webpage was found
            char *currUrl = webpage_getURL(pg); // url of the webpage
            logr("Fetched", depth, currUrl);    // log the fetch

            // create and write to a file in the pageDirectory
            savePage(argv[2], pg, fileNum); // detailed in pagedir.h
            fileNum++;                      

            // scan files for new urls if not at maxDepth
            if (depth < *maxDepth) {
                logr("Scanning", depth, currUrl);
                int pos = 0;
                // get the next URL
                while ((url = webpage_getNextURL(pg, &pos)) != NULL) {
                    // Normalize the URL
                    NormalizeURL(url);
                    logr("Found", depth, url);
                    // check if the URL is internal to the CS50 site
                    char *internal = strstr(url, "http://cs50tse.cs.dartmouth.edu/");
                    if (url == internal){
                        // try to insert the URL into the hashtable
                        if (hashtable_insert(urls, url, "")) {
                            webpage_t *nextPage = webpage_new(url, depth + 1, NULL);
                            if (nextPage == NULL) {
                                fprintf(stderr, "error allocating memory for new webpage");
                                return false;
                            }
                            bag_insert(pages, nextPage);
                            logr("Added", depth, url);
                        } else {    // failed insert indicates duplicate URL
                            logr("IgnDupl", depth, url);
                            free(url);  // free the URL, as called for by webpage_getNextURL
                            url = NULL;
                        }
                    } else {        // non-internal URL
                        logr("IgnExtrn", depth, url);
                        free(url);  // free the URL, as called for by webpage_getNextURL
                        url = NULL;
                    }
                }
            }
        } else {
            printf("fetch failed for url '%s'\n", webpage_getURL(pg));   // unable to grab the html from the url
        }
        webpage_delete(pg);     // delete the webpage, freeing the data
    }
    return true;
}

/* ***************** validateParameters ********************** */
/* checks command-line arguments to ensure the caller has provided
 * 3 arguments: a valid seedUrl, pageDirectory, and depth. Also
 * updates the depth to the provided depth value, if valid
 *
 * We Return:
 *  - True if all parameters are valid
 *  - False if any of the parameters are invalid, or an invalid
 *      number of parameters were provided by the caller
 */
bool validateParameters(int argc, char* argv[], int *depth)
{
    if (argc != 4){     // four arguments: command seedUrl pageDir depth
        fprintf(stderr, "error: invalid parameter count. Usage: crawler seedUrl pageDirectory maxDepth\n");
        return false;
    }

    // validate seed url
    char* url = argv[1];    // pointer to the url string
    char* http = strstr(url, "http://cs50tse.cs.dartmouth.edu");  // if the url starts with the cs50 website, http points to the same place as the url
    if (!(url == http)) {   // check that the pointers are equal
        fprintf(stderr, "error: invalid seedUrl '%s', must be internal to http://cs50tse.cs.dartmouth.edu/\n", argv[1]);
        return false;
    }

    // validate page directory using checkDirectory function from pagedir.c
    if (!checkDirectory(argv[2])) { // function detailed in pagedir.h
        fprintf(stderr, "error: invalid data directory '%s'\n", argv[2]);
        return false;
    }

    // validate depth
    char val;
    if ((sscanf(argv[3], "%d%c", depth, &val)) != 1) {      // ensures depth parameter is solely an integer
        fprintf(stderr, "error: %s is not a valid integer\n", argv[3]);
        return false;
    }
    if (*depth < 0) {       // depth must be greater than or equal to 0 to search the website
        fprintf(stderr, "error: depth must be 0 or greater (depth provided: %d)\n", *depth);
        return false;
    }
    return true;
}

/* ***************** logr ********************** */
/* **Taken from CS50 lecture notes on crawler
 * Function to log the crawler's actions
 */
inline static void logr(const char *word, const int depth, const char *url)
{
    printf("%2d %*s%9s: %s\n", depth, depth, "", word, url);
}

/* ***************** webpageDelete ********************** */
/* Function to delete a webpage; passed to the bag upon deletion
 * Realistically, this should never be called as the bag
 *  should always be empty upon deletion
 */
void webpageDelete(void *wp)
{
    if (wp != NULL) {
        webpage_delete(wp);
    }
}
