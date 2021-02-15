---
layout: page
title: TSE Crawler Design Spec
type: Lecture 
date: 2021-02-01b
---

Recall the [Crawler Requirements Spec](REQUIREMENTS.html); the **crawler** crawls a website and retrieves webpages starting with a specified URL.
It parses the initial webpage, extracts any embedded URLs and retrieves those pages, and crawls the pages found at those URLs, but limits itself to
`maxDepth` hops from the seed URL and to URLs that are 'internal' to the designated CS50 server. When the crawler process is complete, the indexing
of the collected documents can begin.

A Design Spec should contain several sections: 

* User interface
* Inputs and Outputs
* Functional decomposition into modules
* Pseudo code for logic/algorithmic flow
* Dataflow through modules
* Major data structures
* Testing plan

Let's look through each.

### User interface

The crawler's only interface with the user is on the command-line; it must always have three arguments.

```bash
$ crawler seedURL pageDirectory maxDepth
```

- `seedURL` is the top of the website to be crawled, 
- `pageDirectory` is the directory where crawler will store the webpage files it retrieves, and 
- `maxDepth` is the maximum depth into the website that crawler will go. For example, the file at the top of the website (typically `index.html`) is at depth 0.  If that file (webpage) has any links in it, the pages at those links are at depth 1, and so on.

For example, to crawl the (1) designated target for the tse labs, (2) storing the pages found in a subdirectory `data` in the current directory, (3) and to search only depths 0, 1, and 2, we would use this command-line:

``` bash
$ crawler http://cs50tse.cs.dartmouth.edu/tse/ ./data/ 2
```

### Inputs and outputs

*Input:* the inputs are command-line parameters and the contents of the website specified by the first command-line parameter for a depth as
specified by the third. See the User Interface above.

*Output:* Crawler will save each explored webpage to a file, one webpage per file, using a unique `documentID` as the file name.  For example,
the top file of the website would have `documentID=0`, the next webpage access from a link on that top page would be `documentID=1`, and so on.
Within each of these files, crawler writes:

 * the full page URL on the first line,
 * the depth of the page (where the `seedURL` is considered to be depth 0) on the second line,
 * the page contents (i.e., the HTML code), beginning on the third line.

### Functional decomposition into modules

We anticipate the following modules or functions:

 1. *main*, which parses arguments and initializes other modules
 2. *crawler*, which loops over pages to explore, until the list is exhausted
 3. *pagefetcher*, which fetches a page from a URL
 4. *pagescanner*, which extracts URLs from a page and processes each one
 4. *pagesaver*, which outputs a page to the the appropriate file

And some helper modules that provide data structures:

  1. ***bag*** of pages we have yet to explore
  2. ***hashtable*** of URLs we've seen so far

### Pseudo code for logic/algorithmic flow

The crawler will run as follows:

1. start from a command line as shown in the User Interface
2. parse the command line, validate parameters, initialize other modules
3. fetch the webpage at `seedURL` and if it fails , take an appropriate action
4. store that webpage in a file in the `pageDirectory` with `documentID= 0`, with the contents as described in the User Interface above with
depth=0
5. add that page to the ***bag*** of webpages to crawl
6. add that URL to the ***hashtable*** of URLs seen
7. while there are more webpages to crawl,  

	7.1 extract a webpage (URL,depth) item from the ***bag*** of webpages to be crawled
	7.2 **pause for at least one second**
	7.3 use *pagefetcher* to retrieve a webpage for that URL
	7.4 use *pagesaver* to write the webpage to the `pageDirectory` with a unique document ID, as described in the Requirements.
	7.5  if the webpage depth is < `maxDepth`, explore the webpage to find the links it contains:
	
                7.5.1 use *pagescanner* to parse the webpage to extract all its embedded URLs
	        7.5.2 for each extracted URL:
	
                	7.5.2.1. 'normalize' the URL (see below)
	               	7.5.2.2. if that URL is not 'internal' (see below), ignore it;
	               	7.5.2.3. try to insert that URL into the *hashtable* of URLs seen
	                7.5.2.4. if it was already in the table, do nothing;
	                7.5.2.5. if it was added to the table:
	
                                7.5.2.5.1. create a new *webpage* for that URL, at depth+1
	                        7.5.2.5.2. add that new *webpage* to the ***bag*** of webpages to be crawled

**normalize the URL** means to convert it into a clean, canonical form.

As one simple example:

`Http://CS50TSE.CS.Dartmouth.edu//index.html`
becomes
`http://cs50tse.cs.dartmouth.edu/index.html`.

**internal** means the URL stays within the CS50 playground http://old-www.cs.dartmouth.edu/.

A good implementation will not necessarily encode all the above code in a single, deeply-nested function. One part of the Implementation Spec is to break the pseudocode down into a cleanly arranged set of functions.

Notice that our pseudocode says nothing about the order in which it crawls webpages. Since the next webpage to crawl is extracted from a ***bag***, we can't expect any particular crawl order.[^1]

[^1]: Recall that our ***bag*** abstract data structure explicitly denies any promise about the order of items removed from a ***bag***.

That's ok.

The result may or may not be a Breadth-First Search, but for the crawler we don't care about the order as long as we explore everything within the `maxDepth` neighborhood.

The crawler completes and exits when it has nothing left in its ***bag*** - no more pages to be crawled.
The maxDepth parameter indirectly determines the number of pages that the crawler will retrieve.


### Dataflow through modules

 1. *main* parses parameters and passes them to the crawler.
 2. *crawler* uses a ***bag*** to track pages to explore, and ***hashtable*** to track pages seen; when it explores a page it gives the page URL to the *pagefetcher*, then the result to *pagesaver*, then to the *pagescanner*.
 3. *pagefetcher* fetches the contents (HTML) for a page from a URL and returns.
 4. *pagesaver* outputs a page to the appropriate file.
 5. *pagescanner* extracts URLs from a page and returns one at a time.

### Major data structures

Three helper modules provide data structures:

 1. ***bag*** of page (URL, depth) structures
 2. ***set*** of URLs (indirectly used by hashtable)
 3. ***hashtable*** of URLs

A data structure that will be used:

 4. *webpage* which contains all the data read for a given webpage, plus the URL and the depth at which it was fetched

### Testing plan

- *Unit testing*.  A small test program to test each module to make sure it does what it's supposed to do. Note: you don't have to retest the modules you built in Lab3 (assuming you didn't change them).

- *Integration testing*.  Assemble the crawler and test it as a whole.
In each case, examine the output files carefully to be sure they have the contents of the correct page, with the correct URL, and the correct depth.
- Ensure that no pages are missing or duplicated.
- Print "progress" indicators from the crawler as it proceeds (e.g., print each URL explored, and each URL found in the pages it explores) so you can watch its progress as it runs.

A sampling of tests that should be run:

-  Test the program with various forms of incorrect command-line arguments to ensure that its command-line parsing, and validation of those parameters, works correctly.

-  Test the crawler with a `seedURL` that points to a non-existent server.

-  Test the crawler with a `seedURL` that points to a non-internal server.

-  Test the crawler with a `seedURL` that points to a valid server but non-existent page.

1. Crawl a simple, closed set of cross-linked web pages.
Ensure that some page(s) are mentioned multiple times within a page, and multiple times across the set of pages.
Ensure there is a loop (a cycle in the graph of pages).
In such a little site, you know exactly what set of pages should be crawled, at what depths, and you know where your program might trip up.

2. Point the crawler at a page in that site, and explore at depths 0, 1, 2, 3, 4, 5.
Verify that the files created match expectations.

3. Repeat with a different seed page in that same site.
If the site is indeed a graph, with cycles, there should be several interesting starting points.

4. Point the crawler at our Wikipedia playground.
Explore at depths 0, 1, 2.
(It takes a long time to run at depth 2 or higher!) Verify that the files created match expectations.

5. When you are confident that your crawler runs well, test it on a part of our playground or with a greater depth - but be ready to kill it if it
seems to be running amok.

