#include "csx730_ioctl.h"
#include "csx730_meta.h"
#include "csx730_vfs.h"

#ifndef CSX730_EXTRA_H
#define CSX730_EXTRA_H

#define NULL_INODE 0

#define SUCCESS(call) if (!(call)) return false
#define VOID_MATH(math_stuff) (void *)((char *) math_stuff)

/**
 * Utility macro to ceil divide two numbers
 * @param x         The dividend
 * @param y         The divisor
 * @return the cieling of @c x / @c y
 */
// https://stackoverflow.com/a/2745086
#define CEIL_DIV(x, y) (1 + (((x) - 1) / (y)))


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
 * @param disk      pointer to @c disk_t object.
 * @param offset    offset from beginning of disk.
 * @param len       length of data to write.
 * @param data      data to write.
 * @return @c true if successful write; @c false otherwise
 */
bool disk_write(disk_t * disk, size_t offset, size_t len, void * data);

/**
 * Allows for arbitrary read on a @c disk_t object.
 * 
 * @param disk      pointer to @c disk_t object.
 * @param offset    offset from beginning of disk.
 * @param len       length of data to read.
 * @param data      buffer to store data in.
 * @return @c true if successful read; @c false otherwise
 */
bool disk_read(disk_t * disk, size_t offset, size_t len, void * data);

// todo: there should probably be a const somewhere in this header
/**
 * Get the inode by the inode number.
 * 
 * @param ino       the inode number.
 * @param table     the inode table.
 * @return the inode with inode number @p ino, or @c NULL if not allocated.
 */
inode_t * get_inode_ino(size_t ino, inode_t table[]);

/**
 * Get the inode by its path.
 * 
 * @param path      the path of the inode.
 * @param table     the inode table
 * 
 * @return the inode at @p path or @c NULL if it doesn't exist. 
 */
inode_t * get_inode(const char ** path, inode_t table[]);

/**
 * Initialize an inode for use.
 * 
 * @param table         the inode table
 * @param table_size    the size of the inode table
 * @return an unused inode
 */
inode_t * allocate_inode(inode_t table[], size_t table_size);

/**
 * Gets the directory name of the current path.
 * Works similarly to dirname(3), but with @c const char **.
 * Result is dynamically allocated, so should be @c free'd when done.
 * @c free_dirname is provided for this purpose.
 * 
 * @param path      the current path
 * @return the parent path
 */
const char ** dirname(const char ** path);

/**
 * Frees a previously allocated @c dirname
 * 
 * @param path      the @c dirname to deallocate.
 */
void free_dirname(const char ** path);

/**
 * Gets the basename of the current path.
 * Works similarly to basename(3), but with @c const char **.
 * 
 * @param path      the current path
 * @param           name the buffer to store the basename.
 */
void basename(const char **path, const char name[]);

/**
 * @param min_size      minimum number of blocks to allocate.
 * @param table         inode table
 * @param table_size    size of inode table
 * @return free block number
 */
size_t get_free_data_block(disk_t * disk, size_t min_size, inode_t table[], size_t table_size, size_t meta_blocks, size_t ino);

#endif