# external-sort-util
Implementation of External Merge Sort algorithm in C++ .

Used to sort large files for which reading all the data into memory of a single machine would be impossible.

The input file is read in small chunks (which fit into memory of a single machine). Each of them is being sorted and written to a separate temporary file. Then External Merge Sort algorithm is used to merge intermediate files into a single sorted output file.

Assumes that input file consists of doubles, one number per line.
Defualt amount of memory available on the machine is set to 100MB.
