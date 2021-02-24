# IMPLEMENTATION.md - 'implementation' spec for the 'indexer' module
# William Dinauer, Dartmouth CS50 Winter 2021

### Detailed Pseudocode
main
    1. call validateParameters, return non-zero if returns false
    2. call the querier, return non-zero if returns false
    3. return zero

querier
    1. load the index into a hashtable using `loadIndex()`. If the index is NULL, print an error message and return false
    2. while the user has not exited, read a line from stdin using freadlinep
    3. create an int maxWords to be half the length of the line
    4. initialize an array of char pointers equal to the size of maxWords and an int numWords to store the number of words in the array
    5. call `splitline` to split the query into words and store them in the array of char pointers. Also updates numWords to the number of words in the array.
    6. print the normalized query
    7. call `searchMatches` to create a counters which stores docId, score pairs
    8. iterate over the resulting counters to get the number of docIds stored in the resulting counters
    9. print the number of docIds in the counters
    10. print the resulting counters using `printResults`
    11. delete the resulting counters
    12. separate between the next query with repeated '-' characters 
    13. free the line and set it equal to NULL
    14. after all queries have been processed (or the user has exited), delete the index and return true

splitline
    1. initialize two integers: a beginning position (begPos) and end position (endPos) initially set to 0, and a char pointer (beg) set to the start of the line.
    2. normalize the line using `NormalizeWord()`
    3. repeat WHILE true:
    4. increment begPos until we've reached a non-space character.
    5. IF the character is '\0', break from the loop
    6. otherwise, set beg to the address of the line at position begPos
    7. set the endPos to begPos
    8. increment endPos until we've reached a non-alpha character. IF the character is '\0':
        
        8.1: save beg to the words[] array passed from the querier at position = numWords
        8.2: increment numWords, then break

    9. then, if the character is not a space, we reject it for being an invalid character. print an error message and return false.
    10. otherwise, update the character in the line from a space to '\0' to indicate the end of the word
    11. set begPos to the end position plus 1
    12. save beg to the words[] array at position = numWords
    13. increment numWords
    14. after leaving the while loop, return true

searchMatches
    1. initialize two new `counters`: 'result,' which holds the final result to be returned, and 'andSequence' which holds the current and sequence being constructed. 
    'andSequence' is initially set to NULL while 'result' is initialized using `counters_new()`
    2. initialize a boolean wasConj to track wether the last word was "and" or "or", set it to true
    3. FOR every word in the query
        
        3.1: IF the word is "and"
            3.1.1: IF wasConj is true or this is the last word in the query...
                3.1.1.1: print an error, delete the result and andSequence, and return NULL; invalid syntax
            3.1.2: ELSE set wasConj to true

        3.2: ELSE IF the word is "or"
            3.2.1: IF wasConj is true or this is the last word in the query...
                3.2.1.1: print an error, delete the result and andSequence, and return NULL
            3.2.2: ELSE set wasConj to true, merge andSequence and result, delete the andSequence and set it equal to NULL
        
        3.3: ELSE set wasConj to false
        3.4: grab the counters from the hashtable for this word
        3.5: IF andSequence is NULL, initialize it using counters_new() and marge the counters from the index into the andSequence
        3.6: ELSE intersect the andSequence with the counters from the index

    4. perform a final merge of the andSequence into the result
    5. delete the andSequence
    6. return the resulting counters

printResults
    1. create a struct pair called zeros which holds {0, 0}
    2. create an array of pairs equal to the number of docIds in the resulting counters
    3. initialize each pair to be 'zeros'
    4. iterate over the resulting counters to sort pairs and insert them into the array of pairs by passing the function `sortPairs` to `counters_iterate`
    5. FOR every pair
        
        5.1: get the webpage associated with the docId using `getNextWebpage`
        5.2: get the url from the webpage using `webpage_getURL`
        5.3: print the pair on a line in the format: "score: (score) doc (docId): (url)"
        5.4: delete the webpage

sortPairs
    1. construct a `pair` for the current node
    2. start at position 0
    3. continue stepping through the array until the node has been inserted using while (true); we break after a valid insert
    4. get the pair at the current position
    5. IF the current pair's count is less than our node's count, it should be inserted here
    6. IF the pair currently here is zeros, we insert our node's pair and break
    7. ELSE we have to shift all counts less than this count one position to the right to allow for insertion
    8. initialize an integer at the next position, i + 1
    9. WHILE the next pair is not zeros
    
        9.1: store the next pair's count and docId
        9.2: override the next pair with the current pair
        9.3: update the 'here' pair with the stored information of the overwritten next pair
        9.4: increment j and get the next pair

    10. update this pair to the info stored in 'here'
    11. replace the pair at i with the current node's pair; it has already been shifted to the right
    12. break

validateParameters
    1. check that the argument count equals 3
    2. validate the pageDirectory by checking if a file ".crawler" is readable in the directory
    3. validate the indexFilename by attempting to open the file to be read
    4. return true if all parameters are validated, false otherwise

### Definitions
The `querier` module leverages the following functions:
```
main (const int argc, char* argv[])
bool splitline(char *line, char *words[], int *numWords);
counters_t *searchMatches(hashtable_t *index, char *words[], int numWords);
void printResults(counters_t *result, int keyCount, char *crawlDir);
void sortPairs(void *arg, int key, int count);
bool isand(char *word);
bool isor(char *word);
counters_t *andCounters(counters_t *counts, counters_t *andSequence);
void mergeCounters(counters_t *counts, counters_t *result);
void intersectCounts(void *arg, int key, int count);
void unionCounts(void *arg, int key, int count);
bool isZeros(pair_t *pr);
static void ctrs_delete(void *ctrs);
void keycount(void *arg, int key, int count);
static void prompt(void);
int fileno(FILE *stream);

``` 
`main` validates parameters and calls for the querier to run
argc and argv are the command-line arguments passed to validateParameters for validation

`splitline` takes a line, splits it into words with pointers stored in words[], and updates numWords
to equal the number of words in the words[] array.
the function returns false if the query contains an invalid character, true otherwise

`searchMatches` takes the hashtable of the index, the array words[], and the integer numWords.
the index is used to grab the counters necessary for performing checks on the words in the query.
these words come from the words[] array built up by splitLine. numWords is used to ensure
the function only grabs words from valid positions in the words[] array
the function returns a resulting counters with docId, scores

`printResults` takes the result counters, a keyCount, and crawlDir. the key count is used as
a size parameter for the array of pairs created. crawlDir is needed to grab the urls of the
webpages associated with docIds so the urls can be printed to the user.
the function sorts the pairs in the result counters into the array of pairs it creates,
then prints them

`sortPairs` is a function for use in `counters_iterate`. it takes an arg, a key, and a count.
the arg is an array of pairs, allowing the key and count to be formed as a pair and sorted
into this array.

`isand` is a simple function which takes a word and checks if it is "and"
returns true if the word is "and"
returns false if the word is not "and"

`isor` is a simple function which takes a word and checks if it is "or"
returns true if the word is "or"
returns false if the word is not "or"

`andCounters` takes two counters: counters and andSequence, and performs 
an intersection operation on them.
leverages `intersectCounts` to perform the intersection operation.
the function returns a pointer to a new counters; the result of their intersection

`mergeCounters` takes two counters: counts and result, and performs a union operation on them
leverages `unionCounts` to perform the union operation

`intersectCounts` is a function passed to `counters_iterate`
the function takes an arg assumed to be the struct `twoctrs`
the key is used to get the count from the index-provided counters
the minimum between count and this fetched count is compared, and the minimum is
stored in the resulting counters

`unionCounts` is a function passed to `counters_iterate`
arg is the resulting counters
the existing count for this node's key in the resulting counters is added to this node's count
to set a new count in the resulting counters

`isZeros` is a simple function to check if the given pair, pr, is of the form {0, 0}

`validateParameters` uses argc to check for a valid number of arguments and the command-line arguments for validation of argv[1] and argv[2]
returns true if argc is 3, argv[1] is a valid crawler-produced directory, and argv[2] is a file that can be written to
returns false otherwise

`ctrs_delete` casts the void pointer ctrs to a counters, and calls the `counters_delete` method on it
for use as a delete function when deleting the hashtable

`prompt` prints "Query? " if stdin comes from a keyboard, and makes use of `fileno`

### Data Structures
the querier makes use of the hashtable, set, counters, and webpage data structures.
```
typedef struct hashtable {
    long size;
    set_t **array;
} hashtable_t;


typedef struct setnode {
    char *key;      
    void *item;     
    struct setnode *next;
} setnode_t;

typedef struct set {
    struct setnode *head;
} set_t;


typedef struct cntnode {
    int key;        
    int count;      
    struct cntnode *next;
} cntnode_t;

typedef struct counters {
    struct cntnode *head;
} counters_t;


struct URL {
  char* scheme; 
  char* user;       
  char* host;               
  char* path;               
  char* query;                
  char* fragment;
};

typedef struct webpage {
  char *url;                               
  char *html;                              
  size_t html_len;                         
  int depth;    
} webpage_t;
```

detailed descriptions of the hashtable, set, counters, and webpage data structures and their functions
are located in hashtable.h, set.h, and counters.h, and webpage.h

the querier also uses two structs created within the module:
```
struct twoctrs {                // struct to hold two counters
    counters_t *ctrs1; 
    counters_t *ctrs2;
};

typedef struct pair {           // struct to hold a docId, count pair (as in counters)
    int docId;
    int count;
} pair_t;
```
the structs are used to pass multiple variables as a single argument to the `counters_iterate` function

### Security and Privacy Properties
the querier is internal to the TSE project. It relies on the work of the `crawler` module
to produce the files which it reads. Thus, the security of the querier relies on the security
of the crawler

### Error Handling and Recovery

For invalid query statements, the querier simply prints a line detailing the issue to the UI
and frees any allocated memory for the specific query to prevent memory leaks. Thus, queries
with invalid syntax or invalid characters are handled. 

If we run out of memory, the program should still exit without memory leaks.

For invalid parameters, the program simply returns with non-zero exit code and writes a line 
to stderr describing the issue. The querier assumes that the crawlerDirectory and the
indexFilename are both properly formatted, so errors could arise when trying to pull
an index from a non-index created file or a crawler directory with gaps between document ids

### Resource Management
the size of the index stored in the querier depends on the size of the stored index, which is
of course dependant on the size of the crawl.
for a massive index, the querier will use a great deal of resources.

### Persistant Storage
the querier does not create any persistant storage, although it relies on the persistant data
provided by the crawler and the indexer.
