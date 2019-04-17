#ifndef CSX730_IOCTL_H
#define CSX730_IOCTL_H

#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "csx730_stat.h"

#define DISK_BLOCK_SIZE 4096 /**< Block size. */

/**
 * Represents a @c disk_t object backed by a disk image.
 */
typedef struct {
  int    fd;   /**< External file descriptor for the disk image. */
  size_t size; /**< Size, in block, of the disk. */
  bool   open; /**< Whether or not the disk is mounted. */
  stat_t gets; /**< Statistics for @c get (input) operations. */
  stat_t puts; /**< Statistics for @c put (output) operations. */
} disk_t;

/**
 * Opens a disk whose image is located at @p path with the specified
 * @p size. If the image cannot be loaded, then this function
 * returns @c -1 and @c errno is set appropriately.
 *
 * @param disk pointer to @c disk_t object
 * @param path path to disk image within the traditional file system
 * @param size number of blocks to support
 * @return @c true if the disk is successfully opened; @c false otherwise
 *         (in which case, @c errno is set appropriately).
 */
bool csx730_ioctl_open(disk_t * disk, const char * path, size_t size);

/**
 * Gets a block from the disk and writes it to the buffer starting at
 * @c data. The number of underlying bytes written to @c data is
 * @c DISK_BLOCK_SIZE.
 *
 * @param disk  pointer to @c disk_t object
 * @param block the block to get
 * @param data
 * @return @c true if the block is read; @c false otherwise
 */
bool csx730_ioctl_get(disk_t * disk, int block, void * data);

/**
 * Puts a block into the disk from the the buffer starting at
 * @c data. The number of underlying bytes read from @c data is
 * @c DISK_BLOCK_SIZE.
 *
 * @param disk  pointer to @c disk_t object
 * @param block the block to get
 * @param data
 * @return @c true if the block is read; @c false otherwise
 */
bool csx730_ioctl_put(disk_t * disk, int block, void * data);

#endif // CSX730_IOCTL_H
