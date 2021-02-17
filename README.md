# William Dinauer
## Dartmouth CS50 Winter 2021, Tiny Search Engine

GitHub username: William Dinauer

To build, run `make`.

To clean up, run `make clean`.

Note: Testing for crawler expects a directory '../data' to exist

the indexer reads from the ../data directory to create the 'index' file within the 'indexer' directory

testing for indexer leaves behind 4 files: index (the index written to by the indexer), newIndex (the index written to by indextest), 
index.sorted (sorted 'index'), and newIndex.sorted (sorted 'newIndex').
these are all left in the indexer directory, for now

for indextest, we assume that the parameters given are a valid indexer-produced file and a new file
that can be written to. Indextest only checks that there are two command-line arguments provided.
