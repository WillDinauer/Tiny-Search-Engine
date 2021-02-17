# README.md - README file for 'crawler' module
# William Dinauer, Dartmouth CS50 Winter 2021

### Assumptions

Hashtable size was set to 50, under the assumption it is a good size for most of the testing.
For an exceedingly large crawl or a much smaller crawl, a larger or smaller hashtable size may be preferred. 

In addition, I am assuming that logging the crawler's actions is always desired. Thus, I have not included an
ifdef block for the logr.

Beyond these, assumptions should follow the code logic contained in the detailed pseudocode for the crawler

### Compilation

To compile, `make crawler`.

### Testing

The `testing.sh` program tests many edge cases of the set module

To test, `make test`.

To clean up after testing, `make clean`.
