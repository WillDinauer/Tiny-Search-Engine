# IMPLEMENTATION.md - 'implementation' spec for the 'indexer' module
# William Dinauer, Dartmouth CS50 Winter 2021

### Detailed Pseudocode
main
    1. call validateParameters, return with non-zero if false
    2. initialize the hashtable
    3. call createIndex, passing in the hashtable to be constructed as the index
    4. write the index to the indexFile by calling writeIndexToFile, leveraging index.c
    5. delete the hashtable
    6. return with a return code of zero if no errors or issues allocating memory throughout

createIndex
    1. set the docId to 1 to track the next webpage to fetch from the crawler directory
    2. WHILE there is a webpage to fetch
    3. fetch the next webpage by calling `getNextWebpage` with the directory name and the address of the docId, leveraging pagedir.c
    4. print an error message if the webpage is null and return false
    4. get the next word from the returned webpage using `webpage_getNextWord`
    5. normalize the word using `NormalizeWord(word)` from word.c
    6. create a new counters
    7. IF counters is not null (did not run out of memory),
        
        7.1 try to insert the counters into the hashtable with the word as the key
        7.2 on success, iterate the count for the docId in the counters
        7.3 on failure, free the new counters and fetch the existing one for the word. Iterate the count for the docId in the existing counters

    8. ELSE print an error message and return false
    9. free the word and delete the webpage to prevent memory leaks
    10. increment the docId for fetching the next webpage
    11. ultimately return true if all webpages are parsed and words are inserted with no issues

validateParameters
    1. check that the argument count equals 3
    2. validate the pageDirectory, by checking if a file ".crawler" is readable in the directory
    3. validate the indexFilename by attempting to open the file to be written to
    4. return true if all parameters are validated, false otherwise

### Definitions
The `crawler` module leverages the following functions:
```
main (const int argc, char* argv[])
static bool createIndex(hashtable_t *index, char* argv[])
static bool validateParameters(int argc, char* argv[], int *depth);
static void ctrs_delete(void *ctrs);
``` 
`main` initializes the hashtable and calls the major indexer methods.
argc and argv are passed to validateParameters for validation, and argv is passed to createIndex 
so it can reference the directory name (argv[1]) when fetching webpage files

`createIndex` takes a pointer to a hashtable as the inverted index to build into.
the hashtable holds sets of (word, counters) pairs, with each `counters` module holding (docId, count) pairs.
argv allows the indexer to reference the directory name (argv[1]) for fetching the webpage files.
the function returns true upon successfully creating the inverted index, or false if it ran into
a memory allocation error along the way. Upon a memory allocation error, there is no guarantee
of a lack of memory leaks.

`validateParameters` leverages argc to check for a valid number of arguments and the command-line arguments for validation of argv[1] and argv[2]
returns true if argc is 3, argv[1] is a valid crawler-produced directory, and argv[2] is a file that can be written to
returns false otherwise

`ctrs_delete` casts the void pointer ctrs to a counters, and calls the `counters_delete` method on it
for use as a delete function when deleting the hashtable

The `indexer` also makes use of the following functions from `pagedir.c`, a module 
located in the ../common directory:
```
webpage_t *getNextWebpage(char *dir, int *docId);
bool isCrawlerDir(char *dir);
bool checkFile(char *dir, char *fileName, char *openParam);
```
getNextWebpage fetches the webpage in the crawler-produced directory "dir" with the corresponding
file name: docId. the function returns this webpage.

isCrawlerDir takes a string parameter dir and checks it is a crawler-produced directory. We return true if it is an existing directory, or false if it is not.

checkFile checks if a file in the given directory 'dir' with name 'filename' can be opened for
the purposes indicated by the openParam. if the openParam is "r", we check if we can open the file for reading
if the openParam is "w", we check if we can open the file to write to it.

In addition, the following function from `index.c`, a module located in the ../common directory:
```
void writeIndexToFile(char *fName, hashtable_t *index);
```
writeIndexToFile writes the hashtable containing the inverted index data structure (index) 
to the file given by fName

Lastly, the following function from `word.c`, another module located in the ../common directory:
```
void NormalizeWord(char *word);
```
Normalize word takes the parameter `word` and converts it to lowercase.

### Data Structures
the crawler makes use of the hashtable, set, counters, and webpage data structures.
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

### Security and Privacy Properties
the indexer is internal to the TSE project. It relies on the work of the `crawler` module
to produce the files which it reads. Thus, the security of the indexer relies on what is written
by the crawler.

### Error Handling and Recovery
When the indexer encounters an error in memory allocation, it immediately returns non-null. In such
cases, memory leaks are possible and likely to occur. 

For invalid parameters, the program simply returns with non-zero exit code and writes a line 
to stderr describing the issue. The indexer assumes there are no gaps in file numbers. Thus, 
if gaps are present the indexer will assume there is some sort of issue and return without having
fully created the index or writing it to the index file.

### Resource Management
the size of the index created by the indexer is entirely dependant on the crawl performed by the crawler.
the indexer may use a lot of resourcs when creating a massive inverted index as the entire index is
created before anything is deleted or written to the index file. 

### Persistant Storage
The indexer writes to the index file, which persists after termination. 

The details of the file are detailed in Output
