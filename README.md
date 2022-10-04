# ft_ls
The goal of this project is to recode a POSIX ls utility. The mandatory part demand to implement the flags lRtcr and the bonus part demand to add 5 flags.

my implementation support the following flags: RacdilrtuHgnopsf

The difficulty of this project lies in the optimization of the data structures, sorting and retrieval of datas.

## data structures and sorting

The goal here is (for each directory) to retrieve the data of a file, store it and sort it (according to the current flags)

For this part, I've first decided to experiment with a binary search tree which has the advantage of being ordered (so no other sorting is needed) but after a quick benchmark, it appeared that using a heap sort was faster.

Each file in a directory is pushed to a binary heap, then the heap is entirely pop in a vector and this vector is used for futher operations.

In order to avoid intensive use of malloc, the vector and the heap for each file depth are stored in a vector and reused.
