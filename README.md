# csx730-vfs Virtual File System

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
| `csx730_data.h`   | | [File Organization API](#file-organization-api)                           |
| `csx730_meta.h `  | | [Meta-Data API](#meta-data-api)                                           |
| `csx730_vfs.h`    | | [VFS User Space API](#vfs-user-space-api)                                 |
| `csx730_stat.h`   | `csx730_stat.so`  | [Statistics API](#statistics-api)                                         |

You can think of the VFS as a stacked architecture involving the first four APIs in the table.

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
  struct inode * prev;       /**< Previous inode. */
  struct inode * next;       /**< Next inode. */
  struct inode * child;      /**< First child inode. */
  sem_t          sem;        /**< Semaphore. */
} inode_t;
```

```c
typedef struct superblock {
  unsigned int  magic: 32;   /**< The magic number. */
  unsigned long root_ino;    /**< Root inode number. */
} superblock_t;
```

**No other implementation is provided for you.** You are responsible for reading and
writing the inode and superblock data to the disk image using the
[I/O Control & Basic File System API](#io-control--basic-file-system-api).

### VFS User Space API

This API provides the user space functions for interacting with the VFS. Users of
this API can create, edit, and modify regular files and directories within the file system
using a familiar set of functions. Readers whould consult `csx730_vfs.h`
for complete documentation. Here is a summary of the functions provided by the API:

| Function           | Short Description
|--------------------|------------------------------------|
| `csx730_vfs_init`  | Initialize the file system.        |
| `csx730_creat`     | Create a file or directory.        |
| `csx730_open`      | Open a file or directory.          |
| `csx730_unlink`    | Remove a file or directory.        |
| `csx730_stat`      | Get inode information.             |
| `csx730_close`     | Close a file or directory.         |
| `csx730_seek`      | Reposition read/write file offset. |
| `csx730_read`      | Read from a file.                  |
| `csx730_write`     | Write to a file.                   |

**No other implementation is provided for you.** You are responsible for
managing any necessary in-memory data structures as well as reading and
writing the inode and superblock data to the disk image using the
[I/O Control & Basic File System API](#io-control--basic-file-system-api).

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

## Example User Programs

### Example 1

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
