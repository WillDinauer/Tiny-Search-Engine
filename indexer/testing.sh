#!/bin/bash
#
# testing.sh
#
# runs a series of tests for the 'indexer' and 'indextest' modules
#
# William Dinauer, Dartmouth CS50 Winter 2021

# testing indexer for invalid parameters
# no arguments
./indexer
# too many arguments
./indexer ../data index arg
# non-existent directory
./indexer ../exampleDir index
# non-crawler produced directory
./indexer ../libcs50 index

# call with valid parameters
./indexer ../data index

# testing indextest
# no arguments
./indextest
# too many arguments
./indextest index newIndex arg

# call with valid parameters
./indextest index newIndex

# sort each file and compare
gawk -f indexsort.awk index > index.sorted
gawk -f indexsort.awk newIndex > newIndex.sorted

cat index.sorted

cat newIndex.sorted
