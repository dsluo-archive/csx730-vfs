#include "csx730_vfs.h"
#include "csx730_ioctl.h"
#include <semaphore.h>
#include "csx730_extra.h"

struct {
    disk_t disk;
} __global;

bool csx730_vfs_init(const char * disk_image, size_t size) {

    // TODO: calculate space to reserve for inodes + superblock here?

    SUCCESS(csx730_ioctl_open(&__global.disk, disk_image, size));

    superblock_t superblock = {
        .magic=0xDEADBEEF,
        .root=1
    };

    SUCCESS(disk_write(&__global.disk, 0, sizeof(superblock_t), (void *) &superblock));

    inode_t root = {
        .name={'/'},
        .ino=1,
        .dir=true,
        .size=0, // todo: what should this be for directories
        .bno=0,
        .offset=sizeof(superblock_t),
        .atime=0, // todo time of creation
        .prev=0,
        .next=0,
        .child=0,
    };
    // todo: idk how much this should be initialized for.
    SUCCESS(sem_init(&root.sem, true, 1));

    SUCCESS(disk_write(&__global.disk, sizeof(superblock_t), sizeof(inode_t), (void *) &root));

    return true;
}