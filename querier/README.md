# README.md - README file for 'querier' module
# William Dinauer, Dartmouth CS50 Winter 2021

### Assumptions

the querier assumes that if the indexFilename is a readable file, it was produced by the indexer.
this is relevant for creating the index.

Other assumptions follow the guidelines detailed for the querier.

### Compilation

To compile the querier, `make querier`.
To compile fuzzquery, `make fuzzquery`.

### Testing

The `testing.sh` program tests many edge cases of the querier module

To test, `make test`.

To clean up after testing, `make clean`.
