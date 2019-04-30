#include "csx730_vfs.h"
#include "csx730_ioctl.h"
#include <semaphore.h>
#include "csx730_extra.h"
#include <string.h>
#include <stdlib.h>

#define INODE_BLOCK_RATIO 1/100 // inodes occupy 1% of blocks

void cleanup(void) {
    free(__global.table);
}

bool csx730_vfs_init(const char * disk_image, size_t size) {

    if (__global.initialized)
        return false;
    else {
        __global.initialized = true;
        SUCCESS(!atexit(&cleanup));
    }

    // TODO: calculate space to reserve for inodes + superblock here?

    SUCCESS(csx730_ioctl_open(&__global.disk, disk_image, size));

    // initialize inode table.
    __global.meta_blocks = size * INODE_BLOCK_RATIO + (size * INODE_BLOCK_RATIO == 0 ? 1 : 0); // at least 1 block allocated for metadata
    size_t inode_space = __global.meta_blocks * DISK_BLOCK_SIZE - sizeof(superblock_t); // inodes use up everything superblock doesn't
    __global.inode_count = inode_space / sizeof(inode_t);

    // make sure we have enough inodes to cover all the data blocks.
    while (__global.inode_count < size - __global.meta_blocks) {
        __global.meta_blocks++;
        inode_space = __global.meta_blocks * DISK_BLOCK_SIZE - sizeof(superblock_t);
        __global.inode_count = inode_space / sizeof(inode_t);
    }

    inode_space = __global.inode_count * sizeof(inode_t); // trim off extra space not used

    __global.table = calloc(__global.inode_count, sizeof(inode_t));

    superblock_t superblock;
    bool init = disk_read(0, sizeof(superblock_t), (void *) &superblock);

    if (!init || superblock.magic != 0xDEADBEEF) {
        // new disk image; initalize new.
        superblock.magic = 0xDEADBEEF;
        superblock.root = 0;

        SUCCESS(disk_write(0, sizeof(superblock_t), (void *) &superblock));

        // init root inode
        inode_t root = {
            .name="/",
            .ino=1, // inode numbers are 1 + their index in the table; 0 means unallocated/null.
            .dir=true,
            .size=0, // todo: what should this be for directories
            .bno=0,
            .offset=sizeof(superblock_t),
            .atime=0, // todo time of creation
            .prev=NULL_INODE,
            .next=NULL_INODE,
            .child=NULL_INODE,
        };
        // todo: idk how much this should be initialized for.
        // SUCCESS(sem_init(&root.sem, true, 1));

        memcpy(__global.table, &root, sizeof(inode_t));

        SUCCESS(disk_write(sizeof(superblock_t), inode_space, __global.table));
    } else {
        // already initialized; restore from disk.
        SUCCESS(disk_read(sizeof(superblock_t), inode_space, __global.table));
    }

    return true;
}

bool csx730_creat(const char ** path, bool dir) {
    inode_t * inode = get_inode_path(path);

    if (inode != NULL)
        return false;
    
    const char ** parent_path = dirname(path);
    inode_t * parent = get_inode_path(parent_path);
    free_dirname(parent_path);

    if (parent == NULL)
        return false;

    inode_t * new_inode = allocate_inode();

    new_inode->dir = dir;
    basename(path, new_inode->name);

    if (parent->child != NULL_INODE) {
        inode_t * next = get_inode_ino(parent->child);
        while (next->next != NULL_INODE)
            next = __global.table + next->next - 1;
        next->next = new_inode->ino;
        new_inode->prev = next->ino;
    } else {
        parent->child = new_inode->ino;
    }

    new_inode->bno = get_free_data_block(1, new_inode->ino);

    SUCCESS(disk_write(sizeof(superblock_t), __global.inode_count * sizeof(inode_t), __global.table));

    return true;
}

fd_t csx730_open(const char ** path) {
    inode_t * inode = get_inode_path(path);
    if (inode == NULL)
        return -1;

    file_t * file = malloc(sizeof(file_t));
    file->fd = __global.next_fd++;
    file->inode = inode;
    file->offset = 0;
    file->open = true;
    file->next = NULL;
 
    if (__global.files_head == NULL) {
        __global.files_head = file;
        file->prev = NULL;
    } else {
        __global.files_tail->next = file;
        file->prev = __global.files_tail;
    }

    __global.files_tail = file;

    return file->fd;    
}

bool csx730_unlink(const char ** path) {
    inode_t * inode = get_inode_path(path);
    if (inode == NULL) 
        return false;

    const char ** parent_path = dirname(path); 
    inode_t * parent = get_inode_path(parent_path);
    free_dirname(parent_path);

    inode_t * prev = get_inode_ino(inode->prev);
    inode_t * next = get_inode_ino(inode->next);

    if (parent->child == inode->ino) // if this is head of directory list
        parent->child = next != NULL ? next->ino : NULL_INODE;

    if (prev && next) {
        prev->next = next->ino;
        next->prev = prev->ino;
    } else if (prev && !next) {
        prev->next = NULL_INODE;
    } else if (!prev && next) {
        next->prev = NULL_INODE;
    }

    memset(inode, 0, sizeof(inode_t));

    return true;
}

bool csx730_stat(const char ** path, inode_t * inode) {
    inode_t * found = get_inode_path(path);
    if (found != NULL) {
        memcpy(inode, found, sizeof(inode_t));
        return true;
    }
    return false;
}

bool csx730_fstat(fd_t fd, inode_t * inode) {
    file_t * file = get_file_fd(fd);
    if (file != NULL) {
        memcpy(inode, file->inode, sizeof(inode_t));
        return true;
    }
    return false;
}

bool csx730_close(fd_t fd) {
    file_t * file = get_file_fd(fd);
    
    if (file == NULL)
        return false;

    if (__global.files_head == file)
        __global.files_head = file->next;
    if (__global.files_tail == file)
        __global.files_tail = file->prev;

    if (file->prev)
        file->prev = file->next;
    if (file->next)
        file->next = file->prev;
    
    free(file);

    return true;
}

bool csx730_seek(fd_t fd, size_t offset);
ssize_t csx730_read(fd_t fd, void * buf, size_t len);
ssize_t csx730_write(fd_t fd, void * buf, size_t len);