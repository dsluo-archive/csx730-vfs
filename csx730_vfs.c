#include "csx730_vfs.h"
#include "csx730_ioctl.h"
#include <semaphore.h>
#include "csx730_extra.h"
#include <string.h>

#define INODE_BLOCK_RATIO 1/100 // inodes occupy 1% of blocks

struct {
    disk_t disk;
    inode_t * table; // root is always table[0], but inode #1
    size_t table_size;
} __global;

bool csx730_vfs_init(const char * disk_image, size_t size) {

    // TODO: calculate space to reserve for inodes + superblock here?

    SUCCESS(csx730_ioctl_open(&__global.disk, disk_image, size));

    // initialize inode table.
    size_t meta_blocks = size * INODE_BLOCK_RATIO + (size * INODE_BLOCK_RATIO == 0 ? 1 : 0); // at least 1 block allocated for metadata
    size_t inode_space = meta_blocks * DISK_BLOCK_SIZE - sizeof(superblock_t); // inodes use up everything superblock doesn't
    size_t inode_count = inode_space / sizeof(inode_t);

    // make sure we have enough inodes to cover all the data blocks.
    while (inode_count < size - meta_blocks) {
        meta_blocks++;
        inode_space = meta_blocks * DISK_BLOCK_SIZE - sizeof(superblock_t);
        inode_count = inode_space / sizeof(inode_t);
    }

    inode_space = inode_count * sizeof(inode_t); // trim off extra space not used

    __global.table = calloc(inode_count, sizeof(inode_t));
    __global.table_size = inode_count;

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
bool csx730_stat(const char ** path, inode_t * inode) {
    inode_t * found = get_inode(path, __global.table);
    if (found != NULL) {
        memcpy(inode, found, sizeof(inode_t));
        return true;
    }
    return false;
}
