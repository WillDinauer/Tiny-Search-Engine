/*
 * pagedir.c - contains functions related to the data directory in the "Tiny Search Engine" (TSE) project
 * 
 * William Dinauer, Dartmouth CS50 Winter 2021
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "pagedir.h"
#include "webpage.h"
#include "file.h"

/* ***************** checkDirectory ********************** */
/* see pagedir.h for description */
bool checkDirectory(char *dir)
{
    return checkFile(dir, "/.crawler", "w");
}

/* ***************** isCrawlerDir ********************** */
/* see pagedir.h for description */
bool isCrawlerDir(char *dir)
{
    return checkFile(dir, "/.crawler", "r");
}

/* ***************** checkFile ********************** */
/* see pagedir.h for description */ 
bool checkFile(char *dir, char *fname, char *openParam)
{
    FILE *fp;
    int dirLen = strlen(dir);                  // length of the directory's name
    int fileLen = strlen(fname);              // length of the file's name
    char *filename = calloc(dirLen + fileLen + 1, sizeof(char));    // allocate memory to hold directory and file plus '\0'
    if (filename == NULL) { // error allocating memory
        return false;
    }
    strcpy(filename, dir);          // copy the directory name
    strcat(filename, fname);        // concatenate the filename
    // try to open the file based on the openParam; on success clean-up and return true
    if ((fp = fopen(filename, openParam))) {
        fclose(fp);
        free(filename);
        return true;
    }
    // return false if the file could not be opened
    free(filename);
    return false;
}

/* ***************** getNextWebpage ********************** */
/* see pagedir.h for description */
webpage_t *getNextWebpage(char *dir, int *docId)
{
    FILE *fp;
    int dirLen = strlen(dir);
    int numLen = snprintf(NULL, 0, "%d", *docId);
    
    char *numString = calloc(numLen + 1, sizeof(char));
    char *filename = calloc(numLen + dirLen + 2, sizeof(char));
    if (filename != NULL && numString != NULL) {
        // form the string to fetch the file in the given directory
        strcpy(filename, dir);
        snprintf(numString, numLen + 1, "%d", *docId);
        strcat(filename, "/");
        strcat(filename, numString);
        if ((fp = fopen(filename, "r"))) {  // try to open the file
            // read the url, depth, and html from the file
            char *url = freadlinep(fp);
            char *depthAsString = freadlinep(fp);
            char *html = freadfilep(fp);
            int depth = atoi(depthAsString);
            // create and return the new webpage
            webpage_t *wp = webpage_new(url, depth, html);
            free(depthAsString);
            free(numString);
            free(filename);
            fclose(fp);
            return wp;      // returns point to wp; already NULL if ran out of memory
        } else {    // error opening the file
            free(numString);
            free(filename);
            return NULL;
        }
    }       // ran out of memory when allocating memory for numString or filename
    if (filename != NULL) {
        free(filename);
    }
    if (numString != NULL) {
        free(numString);
    }
    return NULL;
}

/* ***************** savePage ********************** */
/* see pagedir.h for description */
void savePage(char *dir, webpage_t *wp, int fileNum)
{
    int depth = webpage_getDepth(wp);   // depth for the given webpage
    char *url = webpage_getURL(wp);     // url for the given webpage
    char *html = webpage_getHTML(wp);   // html for the given webpage
    int numLen = snprintf(NULL, 0, "%d", fileNum);  // length of the fileNum number as a string
    int dirLen = strlen(dir);           // length of the directory name
    FILE *fp;                       

    // allocate memory for the strings which will be concatenated together
    char *numString = calloc(numLen + 1, sizeof(char));
    char *filename = calloc(numLen + dirLen + 2, sizeof(char)); // + 2 for the "/" and trailing '\0'
    if (filename != NULL && numString != NULL) { 
        strcpy(filename, dir);
        snprintf(numString, numLen + 1, "%d", fileNum); // convert fileNum to a string
        strcat(filename, "/");
        strcat(filename, numString);                    // concatenate the strings
        if ((fp = fopen(filename, "w"))) {              // open the file
            // write to the file
            fprintf(fp, "%s\n", url);           // write the url
            fprintf(fp, "%d\n", depth);         // write the depth
            fprintf(fp, "%s\n", html);          // write the html           
            fclose(fp);
        }
    }
    //clean up
    if (filename != NULL) {
        free(filename);
    }
    if (numString != NULL) {
        free(numString);
    }
}
