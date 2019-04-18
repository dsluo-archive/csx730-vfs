#ifndef CSX730_META_H
#define CSX730_META_H

#include <time.h>
#include <semaphore.h>
#include <stdbool.h>

/**
 * Structure for an inode.
 */
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

/**
 * Structure for a superblock. The magic number for a superblock
 * is expected to be @c 0xDEADBEEF.
 */
typedef struct superblock {
  unsigned int  magic: 32;   /**< The magic number. */
  unsigned long root;        /**< Root inode number. */
} superblock_t;

#endif // CSX730_META_H
