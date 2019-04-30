
#include "csx730_vfs.h"
#include "csx730_ioctl.h"

#include <string.h>
#include <stdio.h>
#include "csx730_extra.h"

int main() {

  // your test code here

  disk_t disk;
  const char * path = "/home/dsluo/csx730-vfs/vdisk.img";
  size_t size = 3;

  csx730_ioctl_open(&disk, path, size);

  char data[8192];
  char * numbers = "0123456789";

  for (int i = 0; i < 8192; i++) {
    data[i] = numbers[i%10];
  }

  disk_write(&disk, 2048, 8192, data);

  memset(data, '\0', 8192);

  disk_read(&disk, 2048, 8192, data);

  // csx730_vfs_init(path, size);

  // inode_t inode;

  // const char * path2 = "/";
  // csx730_stat(&path2, &inode);

  return 0;

} // main
