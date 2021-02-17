# DESIGN.md - the design spec for the indexer module
#
# William Dinauer, Dartmouth CS50 Winter 2021

### User interface

The indexer's only interface with the user is on the command-line; it must always have two arguments.

```bash
$ indexer pageDirectory indexFilename
```

- `pageDirectory` is the directory containing all the stored webpage files retrieved by the crawler
- `indexFilename` is the name of the file that the inverted index data structure will be written to

Example Usage:
``` bash
$ indexer ../indexer indexFile
```

### Inputs and outputs

*Input:* the inputs are command-line parametery; the crawler-produced directory name and the index filename. See the User Interface above.

*Output:*  the indexer writes the inverted index data structure it constructs to the index file. Each word is given its own line, followed
by (docId, count) pairs. The docId specifies the corresponding Id number given to a webpage by the crawler, and the count indicates the frequency
of the word in that webpage.
Lines are written in the format:
```
word docId count [docId] [count] ... 
```
### Functional decomposition into modules

We anticipate the following modules or functions:

 1. *main*, which parses arguments and initializes other modules
 2. *indexer*, which constructs the inverted index data structure
 3. *pagedir*, which reads webpages from the crawler produced directory and passes them to the indexer
 4. *index*, which writes the constructed index to the given index filename
 4. *word*, which normalizes a word to be added to the inverted index

And some helper modules that provide data structures:

  2. ***hashtable*** of words mapping to counters, each counter
    of which holds (docId, count) pairs. This hashtable is the inverted
    index data structure

### Pseudo code for logic/algorithmic flow

The indexer will run as follows:

1. start from a command line as shown in the User Interface
2. parse the command line, validate parameters, initialize other modules
3. while there is a still webpage to fetch from the crawler-produced directory,

    3.1 fetch the webpage with docId starting at docId = 1, incremented for each fetch
    3.2 while there is another word to get from the webpage,
        
        3.2.1 normalize the word
        3.2.2 try to add a new counters to the index (the hashtable) with the word as the key
        3.2.3 on success, add the docId to the counters
        3.2.4 on fail, find the existing counters for the word in the index and add the docId to it
    
4. open the index file for writing
5. iterate over each counters in each set for every set in the index to write to the file
    in the format specified in the Output 

### Dataflow through modules

 1. *main* parses parameters and passes them to the indexer.
 2. *indexer* uses a ***hashtable*** to store the inverted index, parsing webpages read from ***pagedir*** to add words normalized by the ***word*** module with an associated counter to the index. The counter tracks the frequency of the word in every docId passed to the indexer.
 3. *pagedir* fetches the URL, depth, and HTML for each webpage written to a file by the crawler, and returns the webpage to the indexer.
 4. *index* writes the index to the index file by iterating over the contained counters and sets
 5. *word* normalizes each word, converting it to lowercase

### Major data structures

Three helper modules provide data structures:

 1. ***counters*** holding (docId, count) pairs
 2. ***set*** of (normalized words, counters) pairs (indirectly used by hashtable)
 3. ***hashtable*** of normalized words and counters, constituting the inverted index

A data structure that will be used:

 4. *webpage* which contains the data (URL, depth , HTLM) for a given webpage written to a file by the crawler

### Testing plan

- *Unit testing*.  A small test program to test each module to make sure it does what it's supposed to do.

- *Integration testing*.  Assemble the indexer and test it as a whole.
- Ensure the index file is formatted correctly with no crawled pages missing or words miscounted

- Ensure the indexer prints proper error messages for a variety of invalid command-line arguments

- Perform indexing for a simple crawl and verify the results in the index file match expected results based on the files in the crawler-produced directory
