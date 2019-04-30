#include "csx730_vfs.h"
#include "csx730_ioctl.h"
#include <semaphore.h>
#include "csx730_extra.h"
#include <string.h>

#define INODE_BLOCK_RATIO 1/100 // inodes occupy 1% of blocks

struct {
    disk_t disk;
    inode_t * table; // root is always table[0], but inode #1
    size_t meta_blocks; // number of blocks for metadata (superblock + all inodes)
    size_t inode_count;
    bool initialized;
} __global;

bool csx730_vfs_init(const char * disk_image, size_t size) {

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
    bool init = disk_read(&__global.disk, 0, sizeof(superblock_t), (void *) &superblock);

    if (!init || superblock.magic != 0xDEADBEEF) {
        // new disk image; initalize new.
        superblock.magic = 0xDEADBEEF;
        superblock.root = 0;

        SUCCESS(disk_write(&__global.disk, 0, sizeof(superblock_t), (void *) &superblock));

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

        SUCCESS(disk_write(&__global.disk, sizeof(superblock_t), inode_space, __global.table));
    } else {
        // already initialized; restore from disk.
        SUCCESS(disk_read(&__global.disk, sizeof(superblock_t), inode_space, __global.table));
    }

    return true;
}

bool csx730_creat(const char ** path, bool dir) {
    inode_t * inode = get_inode(path, __global.table);

    if (inode != NULL)
        return false;
    
    const char ** parent_path = dirname(path);
    inode_t * parent = get_inode(parent_path, __global.table);
    free_dirname(parent_path);

    if (parent == NULL)
        return false;

    inode_t * new_inode = allocate_inode(__global.table, __global.inode_count);

    new_inode->dir = dir;
    basename(path, new_inode->name);

    if (parent->child != NULL_INODE) {
        inode_t * next = __global.table + parent->child - 1;
        while (next->next != NULL_INODE)
            next = __global.table + next->next - 1;
        next->next = new_inode->ino;
    } else {
        parent->child = new_inode->ino;
    }

    new_inode->bno = get_free_data_block(&__global.disk, 1, __global.table, __global.inode_count, __global.meta_blocks, new_inode->ino);

    SUCCESS(disk_write(&__global.disk, sizeof(superblock_t), __global.inode_count * sizeof(inode_t), __global.table));

    return true;
}
bool csx730_stat(const char ** path, inode_t * inode) {
    inode_t * found = get_inode(path, __global.table);
    if (found != NULL) {
        memcpy(inode, found, sizeof(inode_t));
        return true;
    }
    return false;
}
