#include "csx730_extra.h"
#include <string.h>

// https://stackoverflow.com/a/2745086
size_t ceil_div(size_t x, size_t y) {
    return 1 + ((x - 1) / y);
}

bool disk_read(disk_t * disk, size_t offset, size_t len, void * data) {
    size_t block_num = offset / DISK_BLOCK_SIZE; 
    size_t block_offset = offset % DISK_BLOCK_SIZE;
    size_t block_count = ceil_div((len + block_offset), DISK_BLOCK_SIZE);

    char block[DISK_BLOCK_SIZE];

    // first block
    SUCCESS(csx730_ioctl_get(disk, block_num, (void *) block));
    SUCCESS(memcpy(data, &block + block_offset, DISK_BLOCK_SIZE - block_offset));

    // middle blocks
    for (size_t i = 1; i < block_count - 1; i++) {
        SUCCESS(csx730_ioctl_get(disk, i + block_num, VOID_MATH(data + DISK_BLOCK_SIZE * i)));
    }

    // last block
    SUCCESS(csx730_ioctl_get(disk, block_num + block_count, (void *) &block));
    SUCCESS(memcpy(VOID_MATH(data + DISK_BLOCK_SIZE * block_count), &block, block_offset));
    
    return true;
}

bool disk_write(disk_t * disk, size_t offset, size_t len, void * data) {
    size_t block_num = offset / DISK_BLOCK_SIZE; 
    size_t block_offset = offset % DISK_BLOCK_SIZE;
    size_t block_count = ceil_div((len + block_offset), DISK_BLOCK_SIZE);

    char block[DISK_BLOCK_SIZE];

    // first block
    SUCCESS(csx730_ioctl_get(disk, block_num, (void *) &block));
    SUCCESS(memcpy(&block + block_offset, data, DISK_BLOCK_SIZE - block_offset));
    SUCCESS(csx730_ioctl_put(disk, block_num, (void *) &block));

    // middle blocks
    for (size_t i = 1; i < block_count - 1; i++) {
        SUCCESS(csx730_ioctl_put(disk, i + block_num, VOID_MATH(data + DISK_BLOCK_SIZE * i)));
    }

    // last block
    SUCCESS(csx730_ioctl_get(disk, block_num + block_count, (void *) &block));
    SUCCESS(memcpy(&block, VOID_MATH(data + DISK_BLOCK_SIZE * block_count), block_offset));
    SUCCESS(csx730_ioctl_put(disk, block_num + block_count, (void *) &block));
    
    return true;
}