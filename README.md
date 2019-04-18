# csx730-vfs Virtual File System

> I wanted to have virtual memory, at least as it's coupled with file systems.
> __--Ken Thompson__

**DUE TUE 2019-04-30 (Apr 30) @ 11:55 PM**

This repository contains the skeleton code for the `csx730-vfs` project
assigned to the students in the Spring 2019 CSCI 4730/6730 class
at the University of Georgia.

## Academic Honesty

You agree to the Academic Honesty policy as outlined in the course syllabus.
In accordance with this notice, I must caution you **not** to fork this repository
on GitHub if you have an account. Doing so will more than likely make your copy of
the project publicly visible. Please follow the instructions contained in the
[How to Get the Skeleton Code](#how-to-get-the-skeleton-code) section below in
order to do your development on Nike. Furthermore, you must adhere to the copyright
notice and licensing information at the bottom of this document.

## Overview

The `csx730-vfs` project provides a virtual file system (VFS) to user programs that
is similar to the [Unix File System](https://en.wikipedia.org/wiki/Unix_File_System)
in terms of both interface and general layout. User programs can create, edit, and
modify regular files and directories within the file system via a convenient
User Space API. The architecture for the VFS involves multiple APIs, each
defined in their own header file:

| Header            | Implementation    | Role                                                                      |
|-------------------|-------------------|---------------------------------------------------------------------------|
| `csx730_ioctl.h`  | `csx730_ioctl.so` | [I/O Control & Basic File System API](#io-control--basic-file-system-api) |
| `csx730_meta.h `  | | [Meta-Data API](#meta-data-api)                                           |
| `csx730_vfs.h`    | | [VFS User Space API](#vfs-user-space-api)                                 |
| `csx730_stat.h`   | `csx730_stat.so`  | [Statistics API](#statistics-api)                                         |

You can think of the VFS as a stacked architecture involving the first three APIs
in the table. It supports file names up to `255` characters and, theoretically,
file sizes up to `18,446,744,073,709,551,615` bytes (`16,384` PiB). It's
performance is not the best, however, because the inode stuctures are a set of
simple linked lists.

<hr/>

## API Descriptions

### I/O Control & Basic File System API

This API provides block I/O disk emulation. Users of this API can open disk images and
get / read and put / write blocks on the opened disks. **An implementation is provided for you
in `csx730_ioctl.so`.** Readers whould consult `csx730_ioctl.h` for complete documentation.
Here is a summary of the functions provided by the API:

| Function            | Short Description      |
|---------------------|------------------------|
| `csx730_ioctl_open` | Opens a disk image.    |
| `csx730_ioctl_get`  | Gets / reads a block.  |
| `csx730_ioctl_put`  | Puts / writes a block. |

The `disk_t` structure is defined as follows:

```c
typedef struct {
  int    fd;   /**< External file descriptor for the disk image. */
  size_t size; /**< Size, in block, of the disk. */
  bool   open; /**< Whether or not the disk is mounted. */
  stat_t gets; /**< Statistics for @c get (input) operations. */
  stat_t puts; /**< Statistics for @c put (output) operations. */
} disk_t;
```

The `disk_t` objects for opened disks contain `stat_t` objects for bock-level I/O statistics
gathered using the [Statistics API](#statistics-api).

### Meta-Data API

This API provides the inode and superblock structurs. Readers whould consult `csx730_meta.h`
for complete documentation. The `inode_t` and `superblock_t` structures are defined as follows:

```c
typedef struct inode {
  char           name [255]; /**< Name of the file. */
  unsigned long  ino;        /**< Inode number. */
  bool           dir;        /**< Whether or not the file is a directory. */
  size_t         size;       /**< Size of the file. */
  size_t         bno;        /**< Starting block number for file data. */
  size_t         offset;     /**< Offset of file data in starting block. */
  time_t         atime;      /**< Time of last file access. */
  unsigned long  prev;       /**< Previous inode number. */
  unsigned long  next;       /**< Next inode. number. */
  unsigned long  child;      /**< First child inode number. */
  sem_t          sem;        /**< Semaphore. */
} inode_t;
```

```c
typedef struct superblock {
  unsigned int  magic: 32;   /**< The magic number (0xdeadbeef). */
  unsigned long root;        /**< Root inode number. */
} superblock_t;
```

**No other implementation is provided for you.** You are responsible for reading and
writing the inode and superblock data to the disk image using the
[I/O Control & Basic File System API](#io-control--basic-file-system-api).

You have a lot of flexibility when it comes to the organization of both
the VFS meta-data and data within the disk image. The only absolute
requirement, as far as organization is concerned, is that the beginning
of the disk image **must be** the `superblock_t` object with the
appropriate magic number and requisite information. The `inode_t` objects
can be placed wherever you want within the disk image, however, you
will want to put some thought into how you lay them out. Remember,
you will need intermediate in-memory buffers to get / put this information
from / to the disk image via the
[I/O Control & Basic File System API](#io-control--basic-file-system-api).

The actual (regular) file data can also be placed wherever you want
within the disk image. To keep things simple, we'll assume that regular
file data is stored contiguously in the disk image. This may require you
to compact / defragment the disk image from time to time to ensure that
space is available for new files. **A very important consideration is
that a file may be made bigger via a call to `csx730_write`.**

### VFS User Space API

This API provides the user space functions for interacting with the VFS. Users of
this API can create, edit, and modify regular files and directories within the file system
using a familiar set of functions. Readers whould consult `csx730_vfs.h`
for complete documentation. Here is a summary of the functions provided by the API:

| Function            | Short Description
|---------------------|------------------------------------|
| `csx730_vfs_init`   | Initialize the file system.        |
| `csx730_creat`      | Create a file or directory.        |
| `csx730_open`       | Open a file or directory.          |
| `csx730_unlink`     | Remove a file or directory.        |
| `csx730_stat`       | Get inode information.             |
| `csx730_fstat`      | Get inode information.             |
| `csx730_close`      | Close a file or directory.         |
| `csx730_seek`       | Reposition read/write file offset. |
| `csx730_read`       | Read from a file.                  |
| `csx730_write`      | Write to a file.                   |
| `csx730_piostats`   | Print block I/O stats.             |
| `csx730_stat_next`  | Get inode info for next file.      |
| `csx730_stat_child` | Get inode info for first file.     |

**No other implementation is provided for you.** You are responsible for
managing any necessary in-memory data structures as well as reading and
writing the inode and superblock data to the disk image using the
[I/O Control & Basic File System API](#io-control--basic-file-system-api).

You have a lot of flexibility when it comes to the organization of
the in-memory data sructures needed to facilitate the User Space API.
At a minimum, you will need to manage some kind of "open file" table
that maintains read / write offsets per open file -- **it is possible
to open the same file more than once, ** with independent read / write
offsets. Unlike a real VFS, this system is _synchronous_, i.e., all
read / write operations are to be written to the disk image immediately
via the [I/O Control & Basic File System API](#io-control--basic-file-system-api).
In the future, you may explore providing intermediate buffers for the
write operations so that blocks are not written immediately on every
call to `csx730_write`.

Implementors of this API not required to provide a user space function
for regular file truncation, however, you may find it conventient to
write one.

The output format for the `csx730_piostats` function is as follows:

```
Block I/O Stats:
* gets [ count = %zu; sum = %Lf; min = %Lf; mean = %Lf; max = %Lf ]
* puts [ count = %zu; sum = %Lf; min = %Lf; mean = %Lf; max = %Lf ]
```

### Statistics API

This API provides functions for collecting statistics. Users of this API can initialize
a statistics object and collect values. As values are collected, certain summary statistics
are automatically computed and held in the initialized object. Functions are provided for
computing additional statistics. **An implementation is provided for you in `csx730_stat.so`.**
Readers whould consult `csx730_stat.h` for complete documentation.
Here is a summary of the functions provided by the API:

| Function                  | Short Description                                       |
|---------------------------|---------------------------------------------------------|
| `csx730_stat_init`        | Initialize a statistics object (unbiased).              |
| `csx730_stat_init_biased` | Initialize a statistics object.                         |
| `csx730_stat_accept`      | Accepts / collects a new value.                         |
| `csx730_stat_mean`        | Computes the mean of the accepted values.               |
| `csx730_stat_variance`    | Computes the variance of the accepted values.           |
| `csx730_stat_stddev`      | Computes the standard deviation of the accepted values. |

The `val_t` type and `stat_t` structure are defined as follows:

```c
typedef long double val_t;
```

```c
typedef struct {
  bool   unbiased; /**< Whether the estimators are restricted to be unbiased. */
  size_t n;        /**< The number of values accepted. */
  val_t  sum;      /**< The sum of the accepted values. */
  val_t  sumSq;    /**< The sum of the squares of the accepted values. */
  val_t  min;      /**< The minimum value accepted. */
  val_t  max;      /**< The maximum value accepted. */
} stat_t;
```

The `disk_t` objects for opened disks in the
[I/O Control & Basic File System API](#io-control--basic-file-system-api)
contain `stat_t` objects for bock-level I/O statistics gathered using the
[Statistics API](#statistics-api).

<hr/>

## How to Get the Skeleton Code

On Nike, execute the following terminal command in order to download the project
files into sub-directory within your present working directory:

```
$ git clone https://github.com/cs1730/csx730-vfs.git
```

This should create a directory called `csx730-vfs` in your present working directory that contains
the project files. For this project, the only files that are included with the project download
are listed near the top of the page [here](https://github.com/cs1730/csx730-vfs).

If any updates to the project files are announced by your instructor, you can
merge those changes into your copy by changing into your project directory
on Nike and issuing the following terminal command:

```
$ git pull
```

If you have any problems with any of these procedures, then please contact
your instructor via Piazza.

## Project Requirements

This assignment is worth 100 points.

### Functional Requirements

A functional requirement is *added* to your point total if satisfied.
There will be no partial credit for any of the requirements that simply
require the presence of a function related a particular functionality.
The actual functionality is tested using test cases.

1. __(39 points) Project Compiles.__ Your submission compiles and can successfully
   link with object files expecting the symbols defined in all the provided
   header files. Please be aware that the __Build Compliance__ non-functional
   requirement still apply.

1. __(61 points) Implement `csx730_vfs.h` functions in `csx730_vfs.c`.__
   Your `Makefile` is expected to produce the following:

   * __(0.5 points)__ `csx730_vfs.o`
   * __(0.5 points)__ `csx730_vfs.so`

   Programs that link against your `csx730_vfs.o` or `csx730_vfs.so` will also need
   to link against the following shared libraries as well as use `-lrt` and `-lm`:

   * `csx730_ioctl.so`
   * `csx730_stat.so`

   Here is a list of the functions that are required:

   * __( 5 points)__ `csx730_vfs_init`
   * __( 5 points)__ `csx730_creat`
   * __( 5 points)__ `csx730_open`
   * __( 5 points)__ `csx730_unlink`
   * __( 3 points)__ `csx730_stat`
   * __( 2 points)__ `csx730_fstat`
   * __( 5 points)__ `csx730_close`
   * __( 5 points)__ `csx730_seek`
   * __( 5 points)__ `csx730_read`
   * __( 5 points)__ `csx730_write`
   * __( 5 points)__ `csx730_piostats`
   * __( 5 points)__ `csx730_stat_child`
   * __( 5 points)__ `csx730_stat_next`

   The documentation for each function is provided directly in
   the header. You may generate an HTML version of the corresponding
   documentation using the `doc` target provided by the project's `Makefile`.
   Students should not modify the prototypes for these functions in any way--doing
   so will cause the tester used by the grader to fail.

   You are free, _actively encouraged_, and will likely need to write
   other functions, as needed, to support the required set of functions.
   It is recommended that you give private function names a `_csx730_` prefix.


### Non-Functional Requirements

A non-functional requirement is *subtracted* from your point total if not satisfied. In order to
emphasize the importance of these requirements, non-compliance results in the full point amount
being subtracted from your point total. That is, they are all or nothing.

1. __(100 points) Build Compliance:__ Your project must compile on Nike
   using the provided `Makefile` and `gcc` (GCC) 8.2.0. Your code must compile,
   assemble, and link with no errors or warnings. The required compiler
   flags for this project are already included in the provided `Makefile`.

   The grader will compile and test your code using `all` target in
   the provided `Makefile`.

1. __(100 points) Libraries:__ You are allowed to use any of the C standard library
   functions. A reference is provided [here](https://en.cppreference.com/w/c).
   No other libraries are permitted.

1. __(100 points) `SUBMISSION.md`:__ Your project must include a properly formatted
   `SUBMISSION.md` file that includes, at a minimum, the following information:

   * __Author Information:__ You need to include your full name, UGA email, and
     which course you are enrolled in (e.g., CSCI 4730 or CSCI 6730).

   * __Implementation Overview:__ You need to include a few sentences that provide an overview
     of your implementation.

   * __Reflecton:__ You need to provide answers to the following questions:

     1. What do you think was the motivation behind assigning this project in this class?
     1. What is the most important thing you learned in this project?
     1. What do you wish you had spent more time on or done differently?
     1. What part of the project did you do your best work on?
     1. What was the most enjoyable part of this project?
     1. What was the least enjoyable part of this project?
     1. How could your instructor change this project to make it better next time?

   A properly formatted sample `SUBMISSION.md` file is provided that may be modified to
   help fulfill this non-functional requirement.

1. __(25 points) Memory Leaks:__ Your submission should not result in any memory leaks.
   The grader will check this using `valgrind(1)`.

1. __(25 points) Code Documentation:__ Any new functions or macros must be properly documented
   using Javadoc-style comments. An example of such comments can be seen in the souce code
   provided with the project. Please also use inline documentation, as needed, to explain
   ambiguous or tricky parts of your code.

## Submission Instructions

You will still be submitting your project via Nike. Make sure your project files
are on <code>nike.cs.uga.edu</code>. Change into the parent directory of your
project directory. If you've followed the instructions provided in earlier in this
document, then the name of your project directory is likely `csx730-vfs`.
While in your project parent directory, execute the following command:

```
$ submit csx730-vfs csx730
```

If you have any problems submitting your project then please make a private Piazza
post to "Instructors" as soon as possible.

<hr/>

## Appendix - Example User Programs

### Example 1

Here is a simple user program that omits error checking:

```c
// initialize the file system
csx730_vfs_init("some_disk.img", 256);

// setup paths for  /home/root/README.md
const char * home = { "home", NULL };
const char * home_root = { "home", "root", NULL };
const char * home_root_readme = { "home", "root", "README.md", NULL };

// create directories and files
csx730_creat(home, true);
csx730_creat(home_root, true);
csx730_creat(home_root_readme, false);

// open and write to the file
fd_t fd = csx730_open(home_root_readme);
const char * msg = "stuff";
csx730_write(fd, msg, strlen(msg));

// read from the file
char buffer [256];
csx730_read(fd, buffer, 5);
buffer[5] = '\0';
printf("%s\n", buffer);
```

### Example 2

Here is another simple user program that assumes the previous program
has already been executed:

```c
// initialize the file system
csx730_vfs_init("some_disk.img", 256);

// setup paths for /home/root
const char * home_root = { "home", "root", NULL };

// open /home/root
fd_t fd = csx730_open(home_root);

// stat the file
inode_t inode;
csx730_stat(fd, &inode);

if (inode.dir) {

  printf("%s is a directory!\n", inode.name);
  inode_t child;

  if (csx730_stat_child(fd, &child)) {
    printf("first entry in dir is %s\n", child.name);
  } else {
    printf("dirctory is empty!\n");
  } // if

} else {

  printf("%s is a regular file!\n", inode.name);

} // if
```

<br/>

[![License: CC BY-NC-ND 4.0](https://img.shields.io/badge/License-CC%20BY--NC--ND%204.0-lightgrey.svg)](http://creativecommons.org/licenses/by-nc-nd/4.0/)

<small>
Copyright &copy; Michael E. Cotterell and the University of Georgia.
This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-nd/4.0/">Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International License</a> to students and the public.
The content and opinions expressed on this page do not necessarily reflect the views of nor are they endorsed by the University of Georgia or the University System of Georgia.
</small>
