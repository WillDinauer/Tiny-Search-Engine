#!/bin/bash
#
# testing.sh
#
# runs a series of tests for the 'querier' module
#
# William Dinauer, Dartmouth CS50 Winter 2021

# testing querier for invalid parameters
# no arguments
./querier
# too many arguments
./querier ../data/toscrape-depth-2 ../data/toscrape-index-2 extraArg
# non-existent directory
./querier ../exampleDir ../data/toscrape-index-2
# non-crawler produced directory
./querier ../indexer ../data/toscrape-index-2
# non-readable indexFilename
./querier ../data/toscrape-depth-2 index

# call with valid parameters
./querier ../data/toscrape-depth-2 ../data/toscrape-index-2

# perform a fuzzquery, generate 100 queries
./fuzzquery ../data/toscrape-index-2 100 0 | ./querier ../data/toscrape-depth-2 ../data/toscrape-index-2
