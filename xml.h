/*
    FUSE: Filesystem in Userspace
    Copyright (C) 2001-2005  Miklos Szeredi <miklos@szeredi.hu>

    This program can be distributed under the terms of the GNU GPL.
    See the file COPYING.
*/

//#define FUSE_USE_VERSION 22

//#include <fuse.h>

char* xml_get_content(char *path);
static int xml_getattr(char *path, struct stat *stbuf);
static int xml_readdir(char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
static int xml_open(char *path, struct fuse_file_info *fi);
static int xml_read(char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
