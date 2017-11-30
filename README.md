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
memory usage not exceed 12 MB.


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
memory usage not exceed 15 MB


Assignment3 Keyword and Concept Search
=====

Introduction:
------
write a program that search a specific word through a list of files in a given folder, list the file names that have the word and sort them by frequency.

Usage:
-----
make file, get a3search   
then search folder path (Test_Folder/simple), index folder path (Test_Folder/simple.idx), search word (apple).
```
./a3search Test_Folder/simple/ Test_Folder/simple.idx apple
```
so the search result is:
```
file6.txt
file3.txt
file1.txt
file5.txt
```

Limit:
-----
memory usage not exceed 30 MB   

2017 S1 COMP9319 HD   
Assignment1 45/45   
Assignment2 100/100   
Assignment3 16/16   
