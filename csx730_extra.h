#include "csx730_ioctl.h"

#ifndef CSX730_EXTRA_H
#define CSX730_EXTRA_H

#define SUCCESS(call) if (!(call)) return false
#define VOID_MATH(math_stuff) (void *)((char *) math_stuff)

/**
 * Utility macro to ceil divide two numbers
 * @param x         The dividend
 * @param y         The divisor
 * @return the cieling of @c x / @c y
 */
// https://stackoverflow.com/a/2745086
#define ceil_div(x, y) (1 + (((x) - 1) / (y)))


#define MAX(x, y) ((x) > (y) ? (x) : (y))
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

#endif