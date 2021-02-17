# README.md - README file for 'crawler' module
# William Dinauer, Dartmouth CS50 Winter 2021

### Assumptions

Hashtable size was set to 100, under the assumption it is a good size for most of the testing.
For an exceedingly large index from a massive crawl, a larger or smaller hashtable size may be preferred. 

Beyond these, assumptions should follow the guidelines detailed for the indexer

### Compilation

To compile the indexer, `make indexer`.
To compile the indextest, `make indextest`.

### Testing

The `testing.sh` program tests many edge cases of the indexer and indextest modules

To test, `make test`.

To clean up after testing, `make clean`.
