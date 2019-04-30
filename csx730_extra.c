#include "csx730_extra.h"
#include <string.h>


bool disk_read(disk_t * disk, size_t offset, size_t len, void * data) {
    const size_t block_num = offset / DISK_BLOCK_SIZE; 
    const size_t block_offset = offset % DISK_BLOCK_SIZE;
    const size_t block_count = ceil_div(block_offset + len, DISK_BLOCK_SIZE);

    const size_t data_offset = DISK_BLOCK_SIZE - block_offset;

    char buffer[DISK_BLOCK_SIZE];
    for (size_t i = 0; i < block_count; i++) {
        SUCCESS(csx730_ioctl_get(disk, block_num + i, buffer));
        if (i == 0) {
            SUCCESS(memcpy(data, buffer + block_offset, MIN(data_offset, len)));
        } else if (i == block_count - 1) {
            SUCCESS(memcpy((char *) data + data_offset + (i - 1) * DISK_BLOCK_SIZE, buffer, block_count * DISK_BLOCK_SIZE - block_offset - len));
        } else {
            SUCCESS(memcpy((char *) data + data_offset + (i - 1) * DISK_BLOCK_SIZE, buffer, DISK_BLOCK_SIZE));
        }
    }
    
    return true;
}

bool disk_write(disk_t * disk, size_t offset, size_t len, void * data) {
    const size_t block_num = offset / DISK_BLOCK_SIZE; 
    const size_t block_offset = offset % DISK_BLOCK_SIZE;
    const size_t block_count = ceil_div(block_offset + len, DISK_BLOCK_SIZE);

    const size_t data_offset = DISK_BLOCK_SIZE - block_offset;

    char buffer[DISK_BLOCK_SIZE];
    for (size_t i = 0; i < block_count; i++) {
        // todo: do we care if this fails?
        csx730_ioctl_get(disk, block_num + i, buffer);
        if (i == 0) {
            SUCCESS(memcpy(buffer + block_offset, data, MIN(data_offset, len)));
        } else if (i == block_count - 1) {
            SUCCESS(memcpy(buffer, (char *) data + data_offset + (i - 1) * DISK_BLOCK_SIZE, block_count * DISK_BLOCK_SIZE - block_offset - len));
        } else {
            SUCCESS(memcpy(buffer, (char *) data + data_offset + (i - 1) * DISK_BLOCK_SIZE, DISK_BLOCK_SIZE));
        }
        SUCCESS(csx730_ioctl_put(disk, block_num + i, buffer));
    }
    
    return true;
}