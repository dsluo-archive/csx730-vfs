#include "csx730_ioctl.h"

#ifndef CSX730_EXTRA_H
#define CSX730_EXTRA_H

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

#endif