# IMPLEMENTATION.md - 'implementation' spec for the 'crawler' module
# William Dinauer, Dartmouth CS50 Winter 2021

### Detailed Pseudocode
main
    1. initialization
        - create integer variables for hashtable size, url length, and maximum depth
        - hashtable size set to 50, possible to increase/decrease based on projected crawl
    2. validate command-line arguments
        - detailed implementation under 'validateParameters'
        - updates maxDepth
    3. initialize data structures
        - create a bag to hold webpages
        - create a hashtable to store urls. In any given set of the hashtable, the url is stored
            as the key and it is given a dummy item: ""
        - we return non-zero if there is not enough memory to create either data structure
    4. handle the parsed url provided by the caller
        - allocate memory for a string equal to the size of the parsed url plus one to 
            account for the trailing '\0'
        - return non-zero if out of memory
        - copy the parsed url to the newly allocated string, 'url'
    5. create the first webpage from the newly allocated url
        - insert into the bag and hashtable
    6. call the crawler function to crawl the webpage
    7. delete the hashtable and bag data structures, freeing all allocated memory

crawler
    1. initialization
        - create an integer variable to track file numbers
        - include a string for the url and a pointer to current webpage being crawled
    2. WHILE we can still extract webpages from the bag without a NULL return (the bag is not empty):
        3. IF we successfully fetch the webpage's html
            4. save the page's URL, depth, and html data to the page directory
            5. IF the current depth is less than the maximum depth, search for new URLs
                6. WHILE there is another URL to grab
                    7. normalize the URL
                    8. IF internal to the CS50 site
                        9. check for a duplicate url by attempting an insert into the hashtable
                        10. IF successfully inserted,
                            11. Create a new webpage for the url at depth+1, insert into the bag
                                - Return 'false' if memory allocation fails
                        11. ELSE free the url and indicate it is a duplicate
                    9. ELSE free the url and indicate that it is external
        4. delete the webpage's data, freeing allocated memory
    3. return true upon a successfully completed crawli

validateParameters
    1. check that the argument count equals 4
        - one for each of: command seedUrl pageDir depth
    2. validate the seedURL, ensuring it is internal to the cs50 site
        - check that the url starts with the domain name by creating a new pointer with strstr and
            ensuring that the url and the new pointer point to the same place
    3. validate the page directory
        4. try to open a writeable file directoryname/.crawler in the directory
            - returns true on success -> valid directory
            - returns false on failure -> invalid directory
    4. validate the depth argument by running sscanf for an integer and a character, and expecting
        a single match
    5. IF the depth is a valid integer, ensure it is non-negative
    6. return true if all parameters are valid

### Definitions
The `crawler` module leverages the following functions:
```
main (const int argc, char* argv[])
bool crawler(bag_t *pages, hashtable_t *urls, char* argv[], int *maxDepth);
bool validateParameters(int argc, char* argv[], int *depth);
inline static void logr(const char *word, const int depth, const char *url);
void webpageDelete(void *wp);
``` 
main initializes the major data structures and calls the essential crawler functions
argc and argv are passed to validateParameters for validation

crawler takes a pointer to a bag for storing webpages, a hashtable for keeping track of urls, 
the command-line arguments to write to the directory, and the maxDepth for maximum crawling purposes
crawler returns true upon a successful crawl, or false on a memory allocation failure during the crawl

validateParameters leverages argc to check for a valid number of arguments, the command-line arguments 
for validation of argv[1], argv[2], and argv[3], and an integer for tracking the depth
The depth is updated within the validateParameters function to be referenced as the max depth for the crawler
Returns true if all the parameters are valid; false if any are invalid.

logr is a simple function used to track the crawler as it progresses.
logr prints a single line for each call of the form:
depth   word: url

webpageDelete calls the delete function for the webpage.
For use as a delete function when deleting the bag, although it is expected to be empty

The `crawler` also makes use of the following functions from `pagedir.c`, a module 
located in the ../common directory:
```
bool checkDirectory(char *dir);
void savePage(char *dir, webpage_t *wp, int fileNum);
```
checkDirectory takes a string parameter dir and checks if it is a valid directory that
can be written to. We return true if it is an existing directory, or false if it is not.
If it is valid, a file .crawler is left in the directory.

savePage saves a webpage, wp, to the directory dir. The file to be written to is 
named whatever the fileNum is, starting at 1 for the first file in the directory.

### Data Structures
the crawler makes use of the bag, hashtable, and webpage data structures.
```
typedef struct bagnode {
  void *item;           
  struct bagnode *next;       
} bagnode_t;

typedef struct bag {
  struct bagnode *head; 
} bag_t;

typedef struct hashtable {
    long size;
    set_t **array;
} hashtable_t;

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

detailed descriptions of the bag, hashtable, and webpage data structures and their functions 
are located in bag.h, hashtable.h, and webpage.h

### Security and Privacy Properties
We should only be accessing webistes within the cs50 domain. The crawler could be potentially
malicious if we were to send requests at a much higher rate to websites outside of the domain.
Thus, we only follow urls within the cs50 website. 

### Error Handling and Recovery
When the crawler encounters an error in memory allocation, it immediately returns non-null. In such
cases, memory leaks are likely to occur. 

For invalid parameters, the program simply returns with non-zero exit code and writes a line 
to stderr describing the issue. An invalid fetch is not considered an error.

### Resource Management
the crawler is limited to fetching a new webpage every second as to not overload the server
processing the requests. 

### Persistant Storage
The crawler stores a file to the given domain for every unique url it comes across, 
so long as that url is internal to the cs50 domain.

The first line of every file is the webpage's url. The second line is the depth at which the url
was found. The subsequent lines are the page's raw html.

Files are named an integer, starting at documentId = 1. The documentId is incremented by 1 for 
every new file. 
