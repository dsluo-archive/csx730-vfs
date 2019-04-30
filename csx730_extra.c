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

inode_t * get_inode(const char ** path, inode_t table[]) {
    inode_t * curr = table;

    if (!strcmp(path[0], "/") && path[1] == NULL) {
        return curr;
    }

    const char ** pathname = path;
    while (curr->dir && *pathname != NULL) {
        if (curr->child == NULL_INODE)
            return NULL;

        inode_t * next = &table[curr->child - 1];
        while (strcmp(next->name, *pathname)) {
            if (next->next == NULL_INODE)
                return NULL;
            next = &table[next->next - 1];
        }
        curr = next;
        pathname++;
    }

    const char name[256] = "\0";
    basename(path, name);

    return !strcmp(curr->name, name) ? curr : NULL;
}

inode_t * allocate_inode(inode_t table[], size_t table_size) {
    for (size_t i = 0; i < table_size; i++)
        if (table[i].ino == NULL_INODE) {
            table[i].ino = i + 1;
            return &table[i];
        }
    return NULL;
}

void deallocate_inode(inode_t * inode);

const char ** dirname(const char * path[]) {
    size_t pathlen = 0;
    for (const char ** i = path; *i != NULL; i++)
        pathlen++;
    
    if (pathlen == 1) {
        char ** parent = malloc(2 * sizeof(char *));
        parent[0] = malloc(sizeof(const char *));
        const char * root = "/";
        strcpy(parent[0], root);
        parent[1] = NULL;
        return (const char **) parent;
    }

    const char ** parent = calloc(pathlen, sizeof(const char *));
    for (const char ** i = path, ** j = parent; *i != NULL; i++, j++) {
        *j = malloc(strlen(*i));
        memcpy((void*) *j, *i, strlen(*i));
    }
    parent[pathlen] = NULL;

    return parent;
}

void free_dirname(const char ** path) {
    size_t pathlen = 0;
    for (const char ** i = path; *i != NULL; i++)
        pathlen++;
    
    for (const char ** i = path; *i != NULL; i++)
        free((void *) *i);
    free(path);
}

void basename(const char ** path, const char name[]) {
    size_t pathlen = 0;
    for (const char ** i = path; *i != NULL; i++)
        pathlen++;
    
    memcpy((void *) name, path[pathlen - 1], strlen(path[pathlen - 1]));
}
}