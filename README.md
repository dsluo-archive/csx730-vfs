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
| `csx730_medta.h`  | | [Meta-Data API](#meta-data-api)                                           |
| `csx730_vfs.h`    | | [VFS User Space API](#vfs-user-space-api)                                 |
| `csx730_stat.h`   | `csx730_stat.so`  | [Statistics API](#statistics-api)                                         |

You can think of the VFS as a stacked architecture involving the first four APIs in the table.

<hr/>

## API Descriptions

### I/O Control & Basic File System API

This API provides block I/O disk emulation. Users of the is API can open disk images and
get / read and put / write blocks on the opened disks. Readers whould consult 
`csx730_ioctl.h` for complete documentation. Here is a summary of the functions provided 
by the API:

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
gathered using the [Statistics API](#statistics-api). **An implementation is provided for you
in `csx730_ioctl.so`.**

### File Organization API



### Meta-Data API

### VFS User Space API

| Function           | Short Description
|--------------------|------------------------------------|
| `csx730_creat`     | Create a file or directory.        |
| `csx730_open`      | Open a file or directory.          |
| `csx730_unlink`    | Remove a file or directory.        |
| `csx730_stat`      | Get inode information.             |
| `csx730_close`     | Close a file or directory.         |
| `csx730_seek`      | Reposition read/write file offset. |
| `csx730_read`      | Read from a file.                  |
| `csx730_readdir`   | Read from a directory.             |
| `csx730_write`     | Write to a file.                   |

### Statistics API

<hr/>



