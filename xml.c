/*
    FUSE: Filesystem in Userspace
    Copyright (C) 2001-2005  Miklos Szeredi <miklos@szeredi.hu>

    This program can be distributed under the terms of the GNU GPL.
    See the file COPYING.
*/


#define FUSE_USE_VERSION 22

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

static const char *xml_str = "xml World!\n";
static const char *xml_path = "/xml";

char** xml_get_children(const char *path){
    //needs to get children based on current file path
    //returns an array of childnames (char*)
    char* children[3];
    children[0] = "testfile1";
    children[1] = "testfile2";
    children[2] = "testfile3";
    return children;
}

static int xml_getattr(const char *path, struct stat *stbuf)
{
    int res = 0;

    memset(stbuf, 0, sizeof(struct stat));
    if(strcmp(path, "/") == 0) { //root dir
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    }
    else if(strcmp(path, xml_path) == 0) { //at our directory
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = strlen(xml_str);
    }
    else
        res = -ENOENT;

    return res;
}

static int xml_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    (void) offset;
    (void) fi;

    if(strcmp(path, "/") != 0)
        return -ENOENT;

    //add standard paths
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    //get children
    char** children = xml_get_children(path);
    int i;
    for(i=0; i < children.length; i++){
        filler(buf, children[i], NULL, 0);
    }
    filler(buf, xml_path + 1, NULL, 0);

    return 0;
}

static int xml_open(const char *path, struct fuse_file_info *fi)
{
    if(strcmp(path, xml_path) != 0)
        return -ENOENT;

    if((fi->flags & 3) != O_RDONLY)
        return -EACCES;

    return 0;
}

static int xml_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    size_t len;
    (void) fi;
    if(strcmp(path, xml_path) != 0)
        return -ENOENT;

    len = strlen(xml_str);
    if (offset < len) {
        if (offset + size > len)
            size = len - offset;
        memcpy(buf, xml_str + offset, size);
    } else
        size = 0;

    return size;
}

static struct fuse_operations xml_oper = {
    .getattr	= xml_getattr,
    .readdir	= xml_readdir,
    .open	= xml_open,
    .read	= xml_read,
};

int main(int argc, char *argv[])
{
    return fuse_main(argc, argv, &xml_oper);
}

