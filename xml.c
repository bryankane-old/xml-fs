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
#include "xml_parser.h"
#include "xml.h"

// static const char *xml_str = "xml World!\n";
// static const char *xml_path = "/xml";
char* children[3];


char* xml_get_content(char *path){
    //get the content and return as a string (char*)
    char* content;
    int is_attribute = 0;
    node_t* root = open_file(sample_file_name);

    //handle attributes
    char* att = strchr(path, '.');
    if (att){
        //we found a period in the path
        // printf(". found at %d\n", att-path);
        if (path[att - path - 1] == '/'){
            is_attribute = 1;
            // printf("Get content from an attribute\n");
            char clean_path[att-path];
            const char* delim = ".";
            strncpy(clean_path, path, att - path - 1);
            clean_path[att-path-1] = '\0';
            // printf("%s\n", clean_path);
            node_t* node = get_node_at_path(root, clean_path);
            if (node != NULL){
                char *attribute = strtok(att, delim);
                if (strcmp(attribute, "content") == 0){
                    printf("access content\n");
                    content = roxml_get_content(node, NULL, 0, 0);
                } else 
                    content = get_attr_content(node, attribute);
            }
            else
                content = "No Content Here!";
            roxml_close(root);
        }
    }

    if (is_attribute != 1){
        // printf("Made it into non-attribute read\n");
        node_t* node = get_node_at_path(root, path);
        if (node != NULL)
            content = roxml_get_content(node, NULL, 0, 0);
        else
            content = "No content here!";
        roxml_close(root);
    }

    return content;
}



static int xml_getattr(char *path, struct stat *stbuf)
{
    int res = 0;
    memset(stbuf, 0, sizeof(struct stat));
    node_t* root = open_file(sample_file_name);

    //handle attributes
    char* att = strchr(path, '.');
    if (att){
        //we found a period in the path
        // printf(". found at %d\n", att-path);
        if (path[att - path - 1] == '/'){
            // printf("We have an attribute\n");
            stbuf->st_mode = S_IFREG | 0444;
            stbuf->st_nlink = 1;
            stbuf->st_size = strlen(xml_get_content(path));
            return res;
        }
    }

    node_t* node = get_node_at_path(root, path);
    if (node == NULL)
        res = -ENOENT;
    else if(strcmp(path, "/") == 0) { //root dir
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    }
    // else if(is_leaf(path) == -1) { //at our directory
    //     stbuf->st_mode = S_IFREG | 0444;
    //     stbuf->st_nlink = 1;
    //     stbuf->st_size = strlen(xml_get_content(path));
    // }
    else
    {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    }
    // else
    //     res = -ENOENT;

    return res;
}

static int xml_readdir(char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    (void) offset;
    (void) fi;

    // if(strcmp(path, "/") != 0)
    //     return -ENOENT;

    //add standard paths
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    //get children
    int i;
    node_t* root = open_file(sample_file_name);
    node_t* current_dir = get_node_at_path(root, path);
    int num_children = roxml_get_chld_nb(current_dir);
    char* children[num_children];
    get_all_child_file_names(children, current_dir);
    for(i=0; i < num_children; i++){
        filler(buf, children[i], NULL, 0);
        free(children[i]);
    }

    //get attributes
    int num_attributes = roxml_get_attr_nb(current_dir);
    char** attributes = get_all_attributes(current_dir);
    for(i=0; i < num_attributes; i++){
        char attribute[255];
        strcpy(attribute, ".");
        strcat(attribute, attributes[i]);
        filler(buf, attribute, NULL, 0);
        free(attributes[i]);
    }
    if (is_leaf(path) == -1)
        filler(buf, ".content", NULL, 0);
    roxml_close(root);
    return 0;
}

static int xml_open(char *path, struct fuse_file_info *fi)
{
    // if(strcmp(path, xml_path) != 0)
    //     return -ENOENT;

    if((fi->flags & 3) != O_RDONLY)
        return -EACCES;

    return 0;
}

static int xml_read(char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    size_t len;
    (void) fi;
    // if(strcmp(path, xml_path) != 0)
    //     return -ENOENT;
    char* content = xml_get_content(path);

    len = strlen(content);
    if (offset < len) {
        if (offset + size > len)
            size = len - offset;
        memcpy(buf, content + offset, size);
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
    save_initial_file_names(NULL);
    return fuse_main(argc, argv, &xml_oper);
}

