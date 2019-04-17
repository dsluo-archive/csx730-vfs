# csx730-vfs Virtual File System

## Overview

The `csx730-vfs` project provides a virtual file system (VFS) to user programs that
is similar to the [Unix File System](https://en.wikipedia.org/wiki/Unix_File_System)
in terms of both interface and general layout. User programs can create, edit, and
modify regular files and directories within the file system via a convenient
User Space API. The stacked architecture for the VFS involves multiple APIs, each 
defined in their own header file:

| Header            | Role                                                                      |
|-------------------|---------------------------------------------------------------------------|
| `csx730_ioctl.h`  | [I/O Control & Basic File System API](#io-control--basic-file-system-api) |
| `csx730_data.h`   | [File Organization API](#file-organization-api)                           |
| `csx730_medta.h`  | [Meta-Data API](#meta-data-api)                                           |
| `csx730_vfs.h`    | [VFS User Space API](#vfs-user-space-api)                                 |

<hr/>

## API Descriptions

### I/O Control & Basic File System API

### File Organization API

### Meta-Data API

### VFS User Space API

<hr/<>

