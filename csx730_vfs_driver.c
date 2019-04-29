
#include "csx730_vfs.h"
#include "csx730_ioctl.h"

#include <string.h>
#include <stdio.h>

int main() {

  // your test code here

  disk_t disk;
  const char path[4096] = "/home/dsluo/vdisk.img";
  size_t size = 20;
  csx730_ioctl_open(&disk, path, size);

  // disk_write(&disk, 0, strlen(path), (void *) path);
  csx730_ioctl_put(&disk, 0, (void *) &path);

  char buffer[4096];
  csx730_ioctl_get(&disk, 0, (void *) buffer);
  printf(buffer);
  return 0;

} // main
