#!/bin/bash
#
# testing.sh
#
# runs a series of tests for the crawler module
#
# William Dinauer, Dartmouth CS50 Winter 2021

# testing for invalid parameters
# no arguments
./crawler
# too many arguments
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/ 1 extra
# invalid seedUrl
./crawler this.example.com ../data/ 1
# invalid page directory
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html exampledir 1
# invalid depth
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/ inval
# negative depth
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/ -1

# non-existent seedUrl
./crawler http://cs50tse.cs.dartmouth.edu/randomexampleurl ../data 1

# non-internal seedUrl
./crawler http://wikipedia.org ../data/ 1

# simple crawl
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/ 5
# print file 1
cat ../data/1
# file 2
cat ../data/2
# file 3
cat ../data/3

# new starting point
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/D.html ../data/ 5
# file 1
cat ../data/1
# file 2
cat ../data/2
# file 3
cat ../data/3

# wikipedia crawl - a longer crawl. Comment out unless a more extensive crawl is desired
# ./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/ ../data/ 1
# cat ../data/1
