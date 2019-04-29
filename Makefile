CC=gcc
AS=as
CFLAGS=-std=c17 -g -Wall -O0 -pedantic-errors -fPIC
RM=rm -f

.PHONY: clean all

all: csx730_vfs_driver

csx730_vfs_driver: csx730_vfs_driver.o csx730_vfs.so csx730_stat.so csx730_ioctl.so
	$(CC)  -o csx730_vfs_driver \
	csx730_vfs_driver.o \
	csx730_vfs.so csx730_stat.so csx730_ioctl.so \
	-lrt -lm

csx730_vfs_driver.o: csx730_vfs_driver.c csx730_vfs.h csx730_meta.h csx730_stat.h
	$(CC) $(CFLAGS) -c -o csx730_vfs_driver.o csx730_vfs_driver.c

csx730_vfs.so: csx730_vfs.o csx730_extra.o
	$(CC) -shared -o csx730_vfs.so csx730_vfs.o csx730_extra.o

csx730_vfs.o: csx730_vfs.c csx730_vfs.h csx730_meta.h csx730_stat.h csx730_extra.h
	$(CC) $(CFLAGS) -c -o csx730_vfs.o csx730_vfs.c

csx730_extra.o: csx730_extra.c csx730_extra.h
	$(CC) $(CFLAGS) -c -o csx730_extra.o csx730_extra.c

clean:
	$(RM) csx730_vfs.so
	$(RM) csx730_vfs.o
	$(RM) csx730_vfs_driver.o
	$(RM) csx730_vfs_driver
	$(RM) csx730_extra.o
