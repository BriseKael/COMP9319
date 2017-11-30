# COMP9319

Assignment1 RLE
=====

Introduction:
-----
write a sample RLE encoder and decoder.

Usage:
-----
make file, get rlencode and rldecode.
```
make
```
then copy the samples into a same folder, then rlencode is to encode a text file (sample1.txt) into a rle file (rsample1.rle).
```
./rlencode sample1.txt rsample1.rle
```
you will see a rle file (rsample1.rle) in the current folder.   
and rldecode is to decode a rle file (rsample1.rle) into a text file (osample1.txt).
```
./rldecode rsample1.rle osample1.txt
```
you will see a text file (osample1.txt) in the current folder.

Limit:
-----
memory usage not exceed 2 MB.


Assignment2 BWT Search
=====

Introduction:
-----
write a program that search a specific pattern in a given BWT format file. The original file is in format '(\[\d+\]\w+)+'.

Usage:
-----
make file, get bwtsearch  
then search a pattern in bwt files. you also need to input an index file path.
```
./bwtsearch sample/simple1.bwt simple1.idx an
```
the search pattern is 'an', the original file is:
```
[2]banana
```
so the search result is:
```
[2]banana
[2]banana
```
as we have two 'an' in banana.  
for the cases that the file is very big (not over 200 MB), we first create an index file to store BWT index, then use the index file to search the pattern.

Limit:
-----
memory usage not exceed 20 MB


Assignment3 Keyword and Concept Search
=====

Introduction:
------

