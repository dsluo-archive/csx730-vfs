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
  struct inode * prev;       /**< Previous inode. */
  struct inode * next;       /**< Next inode. */
  sem_t          sem;        /**< Semaphore. */
} inode_t;

#endif // CSX730_META_H
