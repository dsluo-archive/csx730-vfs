#include "csx730_vfs.h"
#include "csx730_ioctl.h"

#include <string.h>
#include <stdio.h>
#include "csx730_extra.h"

int main()
{

    // your test code here

    // disk_t disk;
    // const char * path = "/home/dsluo/csx730-vfs/vdisk.img";
    // size_t size = 20;

    // initialize the file system
    csx730_vfs_init("vdisk.img", 20);

    // setup paths for  /home/root/README.md
    const char *home[] = {"home", NULL};
    const char *home_root[] = {"home", "root", NULL};
    const char *home_root_readme[] = {"home", "root", "README.md", NULL};
    const char *home_root_readme2[] = {"home", "root", "README2.md", NULL};

    // create directories and files
    csx730_creat(home, true);
    csx730_creat(home_root, true);
    csx730_creat(home_root_readme, false);
    csx730_creat(home_root_readme2, false);

    // open and write to the file
    fd_t fd = csx730_open(home_root_readme);
    const char *msg = "stuff";
    csx730_write(fd, (void *)msg, strlen(msg));

    const char *msg2 = "thing";
    fd_t fd3 = csx730_open(home_root_readme2);
    csx730_write(fd3, (void *)msg2, strlen(msg));

    // read from the file
    char buffer[256];
    csx730_seek(fd, 0);
    csx730_read(fd, buffer, 5);
    buffer[5] = '\0';
    printf("%s\n", buffer);

    char buffer2[256];
    csx730_seek(fd3, 0);
    csx730_read(fd3, buffer2, 6);
    buffer2[5] = '\0';
    printf("%s\n", buffer2);

    // 2

    // open /home/root
    fd_t fd2 = csx730_open(home_root);

    // stat the file
    inode_t inode;
    csx730_fstat(fd2, &inode);

    if (inode.dir)
    {

        printf("%s is a directory!\n", inode.name);
        inode_t child;

        if (csx730_stat_child(fd2, &child))
        {
            printf("first entry in dir is %s\n", child.name);
        }
        else
        {
            printf("dirctory is empty!\n");
        } // if
    }
    else
    {
        printf("%s is a regular file!\n", inode.name);
    } // if

    return 0;

} // main
