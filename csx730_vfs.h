#ifndef CSX730_VFS_H
#define CSX730_VFS_H

typedef int fd_t;

typedef struct {
  int x;
} inode_t;

fd_t csx730_open(const char ** path);
void csx730_stat(fd_t fd, inode_t * inode);
void csx730_close(fd_t fd);

ssize_t csx730_seek(fd_t fd, size_t len);
ssize_t csx730_read(fd_t fd, void * buf, size_t len);
ssize_t csx730_write(fd_t fd, void * buf, size_t len);

void csx730_creat(const char ** path);
void csx730_mkdir(const char ** path);
void csx730_unlink(const char ** path);

#endif // CSX730_VFS_H
