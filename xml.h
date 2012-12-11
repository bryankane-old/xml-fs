/*
    FUSE: Filesystem in Userspace
    Copyright (C) 2001-2005  Miklos Szeredi <miklos@szeredi.hu>

    This program can be distributed under the terms of the GNU GPL.
    See the file COPYING.
*/

# define FUSE_USE_VERSION 22

#include <fuse.h>

char* xml_get_content(const char *path);
static int xml_getattr(const char *path, struct stat *stbuf);
static int xml_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
static int xml_open(const char *path, struct fuse_file_info *fi);
static int xml_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
