# DESIGN.md - the design spec for the querier module
#
# William Dinauer, Dartmouth CS50 Winter 2021

### User interface

The querier executes from the command-line with two arguments:

```bash
$ ./querier pageDirectory indexFilename
```
- `pageDirectory` is the directory containing all the stored webpage files retrieved by the crawler
- `indexFilename` is the name of the file created by the indexer

The querier displays "Query? " to the user, and will display a normalized form of their query
as well as the matches to their query in a line-by-line format.
Queries are separated by a repeated set of '-'. 

Example Usage:
``` bash
$ ./querier ../data/toscrape-depth-2 ../data/toscrape-index-2
```

### Inputs and outputs

*Input:* the inputs are command-line parametery; the crawler-produced directory name and the index filename. See the User Interface above.
Further inputs are queries, each of which must only contain alpha characters and/or spaces. 

*Output:*  the querier spits back each query in a normalized form, converting uppercase letters to lowercase and trimming excess whitespace between words.
The querier will print a message if the query contains an invalid character or invalid syntax. If the query is of valid syntax, the querier will indicate how
many documents match the query and print scores line by line in the following format:
```
score: (score) doc (docId): (url)
```
Where the docId is the document's associated Id in the pageDirectory and indexFilename, the score is the score for the query, and the url is the url
associated with the webpage with the given docId.

### Functional decomposition into modules

We anticipate the following modules or functions:

 1. *main*, which parses arguments and initializes other modules
 2. *querier*, which constructs the inverted index data structure and continually processes queries until the user escapes
 3. *splitline*, which normalizes a query and splits it into an array of words to process
 4. *searchMatches*, which searches the index to score relevant docIds
 5. *resultPrinter*, which sorts the resulting docId, count pairs in descending order into an array and prints to the UI

And some helper modules that provide data structures:

 1. ***hashtable*** of words mapping to counters, each counter
    of which holds (docId, count) pairs. This hashtable is the inverted
    index data structure
 2. ***counters*** of docId, count pairs to keep track of scores for each docId in a given query.

### Pseudo code for logic/algorithmic flow

The querier will run as follows:

1. start from a command line as shown in the User Interface
2. parse the command line, validate parameters, initialize other modules
3. load the index from the indexFilename
4. continue to read lines until the user escapes (ctrl+d)
5. normalize the query and split the line into an array of words
6. construct the resulting `counters` module by searching the index given the words in the query
7. iterate over the counters to check how many documents match
8. sort the counters by count (score) in descending order
9. print the sorted counters to the UI line-by-line in the format specified in the Output

### Dataflow through modules

 1. *main* parses parameters and passes them to the indexer.
 2. *querier* uses a ***hashtable*** to store the inverted index, stores the array of words created by *splitline*, stores the resulting ***counters*** from a function call to *searchMatches*, and calls for *resultPrinter* to print the stored ***counters*** to the UI.
 3. *splitline* builds into the empty array of words passed by the *querier*, and updates a variable 'numWords' to ensure the querier does not reference an empty position in the array
 4. *searchMatches* takes the array of words and the index to score documents, passing back a ***counters*** which stores docId, score pairs
 5. *resultPrinters* takes the resulting ***counters*** created by *searchMatches*, sorts the pairs by their score in descending order, and prints the pairs line-by-line. 

### Major data structures

Three helper modules provide data structures:

 1. ***counters*** holding (docId, count) pairs
 2. ***set*** of (normalized words, counters) pairs (indirectly used by hashtable)
 3. ***hashtable*** of normalized words and counters, constituting the index

A data structure that will be used:

 4. *webpage* which contains the data (URL, depth , HTLM) for a given webpage written to a file by the crawler. Used to grab the url when printing each docId, score pair.

Some helper structs to facilitate coupling between functions:

 5. *twoctrs*, which holds two ***counters***.
 6. *pair*, which holds two integers: a docId, and a count. Used when sorting the resulting ***counters***.

### Testing plan

- *Unit testing*.  A small test program to test each module to make sure it does what it's supposed to do.

- *Integration testing*.  Assemble the querier and test it as a whole.
- Ensure the querier prints proper error messages for a variety of invalid command-line arguments and queries

- *Fuzz testing*. Generate a variety of random queries to ensure the querier can handle complex syntax. 
