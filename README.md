# ft_ls
The goal of this project is to recode a POSIX ls utility. The mandatory part demand to implement the flags lRtcr and the bonus part demand to add 5 flags.

my implementation support the following flags: RacdilrtuHgnopsf

:warning: all C projects at 42 allows only a certain number of standard function, this is why I use my "libft" a library that contain a lot of function including a lot of reimplementation of POSIX function. While implementing ft_ls, I spent a lot of time to create and optimize functions from this lib (for example the printf family and the containers)

The difficulty of this project lies in the optimization of the data structures, sorting and retrieval of datas.

## data structures and sorting

The goal here is (for each directory) to retrieve the data of a file, store it and sort it (according to the current flags)

For this part, I've first decided to experiment with a binary search tree which has the advantage of being ordered (so no other sorting is needed) but after a quick benchmark, it appeared that using a heap sort was faster.

Each file in a directory is pushed to a binary heap, then the heap is entirely pop from the heap and pushed in a vector that is used for further operations.

In order to avoid intensive use of malloc, the vector and the heap for each directory depth are stored in a vector and reused. (has shown in the diagram below)
![a](https://github.com/jgiron42/ft_ls/blob/78eaa758c2590240b5e0203b7693462fb58b7bde/ft_ls.png)

An other benchmark showed that storing the files structs directly in the vector and heap was slower than storing pointers to files struct (memcpying datas is probably longer than the chunk caching of malloc).

## data retrieval

In order to minimize the time taken by the data retrieval, I used perf to compare my program with GNU ls and see which of my function took too much time to execute.

A good example of optimization is the function get_id_cache (used to retrieve the username and group name from uid and gid). My first approach was to just call getgrgid and getpwuid which took around 40% of the execution time. Since there are far fewer users than files in a computer, I had the idea to create a cache based on binary search tree which reduce the time to aproximately 0.02% of the execution time.

