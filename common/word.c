/*
 * word.c - 'word' module for use in the Tiny Search Engine (TSE) project
 *
 * William Dinauer, Dartmouth CS50 Winter 2021
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

/* ***************** NormalizeWord ********************** */
/* detailed in word.h */
void NormalizeWord(char *word)
{
    int i;
    for (i = 0; word[i] != '\0'; i++) {
        word[i] = tolower(word[i]);      //convert each character to lower case
    }
}
