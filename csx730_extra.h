#include "csx730_ioctl.h"
#include "csx730_meta.h"
#include "csx730_vfs.h"

#ifndef CSX730_EXTRA_H
#define CSX730_EXTRA_H

/**
 * Represents an unallocated inode.
 */
#define NULL_INODE 0

/**
 * Returns false if @p call is false; continues execution otherwise.
 */
#define SUCCESS(call) \
    if (!(call))      \
    return false

/**
 * Represents an open file.
 */
typedef struct file
{
    inode_t *inode;     // file's inode
    fd_t fd;            // file descriptor
    size_t offset;      // current offset
    struct file *next;  // next file in file list
    struct file *prev;  // prev file in file list
} file_t;

/**
 * Represents the global state of the disk. Can only use one disk at a time.
 */
struct
{
    disk_t disk;        // the disk
    inode_t *table;     // root is always table[0], but inode #1
    size_t meta_blocks; // number of blocks for metadata (superblock + all inodes)
    size_t inode_count; // number of inodes available.
    bool initialized;   // if the disk has been initialized.
    file_t *files_head; // head of file list
    file_t *files_tail; // tail of file list
    fd_t next_fd;       // file descriptor of next file to be opened.
} __global;

/**
 * Utility macro to ceil divide two numbers
 * @param x         The dividend
 * @param y         The divisor
 * @return the cieling of @c x / @c y
 */
// https://stackoverflow.com/a/2745086
#define CEIL_DIV(x, y) (1 + (((x)-1) / (y)))

/**
 * Utility macro that returns the larger of the two arguments.
 */
#define MAX(x, y) ((x) > (y) ? (x) : (y))

/**
 * Utility macro that returns the smaller of the two arguments.
 */
#define MIN(x, y) ((x) < (y) ? (x) : (y))

/**
 * Allows for arbitrary write on a @c disk_t object.
 * 
 * @param offset    offset from beginning of disk.
 * @param len       length of data to write.
 * @param data      data to write.
 * @return @c true if successful write; @c false otherwise
 */
bool disk_write(size_t offset, size_t len, void *data);

/**
 * Allows for arbitrary read on a @c disk_t object.
 * 
 * @param offset    offset from beginning of disk.
 * @param len       length of data to read.
 * @param data      buffer to store data in.
 * @return @c true if successful read; @c false otherwise
 */
bool disk_read(size_t offset, size_t len, void *data);

/**
 * Defrags the disk to make more space.
 */
bool disk_defrag();

// todo: there should probably be a const somewhere in this header
/**
 * Get the inode by the inode number.
 * 
 * @param ino       the inode number.
 * @return the inode with inode number @p ino, or @c NULL if not allocated.
 */
inode_t *get_inode_ino(size_t ino);

/**
 * Get the inode by its path.
 * 
 * @param path      the path of the inode.
 * 
 * @return the inode at @p path or @c NULL if it doesn't exist. 
 */
inode_t *get_inode_path(const char **path);

/**
 * Initialize an inode for use.
 * 
 * @return an unused inode
 */
inode_t *allocate_inode();

/**
 * Gets the directory name of the current path.
 * Works similarly to dirname(3), but with @c const char **.
 * Result is dynamically allocated, so should be @c free'd when done.
 * @c free_dirname is provided for this purpose.
 * 
 * @param path      the current path
 * @return the parent path
 */
const char **dirname(const char **path);

/**
 * Frees a previously allocated @c dirname
 * 
 * @param path      the @c dirname to deallocate.
 */
void free_dirname(const char **path);

/**
 * Gets the basename of the current path.
 * Works similarly to basename(3), but with @c const char **.
 * 
 * @param path      the current path
 * @param           name the buffer to store the basename.
 */
void basename(const char **path, const char name[]);

/**
 * Gets a data block that does not have an inode associated with it.
 * 
 * @param min_size      minimum number of blocks to allocate.
 * @param ino           the inode number that we are finding a data block for.
 * @return free block number
 */
size_t get_free_data_block(size_t min_size, size_t ino);

/**
 * Gets a @c file_t from its file descriptor.
 * 
 * @param fd        The file descriptor.
 * @return a pointer to the file associated with @p fd.
 */
file_t *get_file_fd(fd_t fd);

#endif