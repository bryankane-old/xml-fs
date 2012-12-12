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

node_t* root;



char* xml_get_content(char *path){
    //get the content and return as a string (char*)
    char* content;
    int is_attribute = 0;

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
        }
    }

    if (is_attribute != 1){
        // printf("Made it into non-attribute read\n");
        node_t* node = get_node_at_path(root, path);
        if (node != NULL)
            content = roxml_get_content(node, NULL, 0, 0);
        else
            content = "No content here!";
    }

    return content;
}



static int xml_getattr(char *path, struct stat *stbuf)
{
    int res = 0;
    memset(stbuf, 0, sizeof(struct stat));

    //handle attributes
    char* att = strchr(path, '.');
    if (att){
        //we found a period in the path
        // printf(". found at %d\n", att-path);
        if (path[att - path - 1] == '/'){
            // printf("We have an attribute\n");
            stbuf->st_mode = S_IFREG | 0755;
            stbuf->st_nlink = 1;
            stbuf->st_size = strlen(xml_get_content(path));
            return res;
        }
    }

    node_t* node = get_node_at_path(root, path);
    if (node == NULL) {
        printf("Error with: %s\n", path);
        res = -ENOENT;
    }
    else if(strcmp(path, "/") == 0) { //root dir
        // char* perm = get_attr_content(node, "permissions");
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    }
    
    else
    {
        char* perm = get_attr_content(node, "permissions");
        char* time = get_attr_content(node, "modified");
        char* uid = get_attr_content(node, "uid");
        char* gid = get_attr_content(node, "gid");
        // printf("perm: %s\n", perm);
        // printf("other: %o\n", S_IFDIR | 0755);
        stbuf->st_mode = strtol(perm, NULL, 8);
        stbuf->st_mtime = atoi(time);
        stbuf->st_uid = atoi(uid);
        stbuf->st_gid = atoi(gid);
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
    if (is_leaf(root, path) == -1)
        filler(buf, ".content", NULL, 0);
    return 0;
}

static int xml_open(char *path, struct fuse_file_info *fi)
{
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


static int xml_write(char *path, char* buf, size_t size, off_t offset, struct fuse_file_info *fi){
    printf("%s %s\n", path, buf);
    char* att = strchr(path, '.');
    if (att){
        //we found a period in the path
        // printf(". found at %d\n", att-path);
        if (path[att - path - 1] == '/'){
            // is_attribute = 1;
            printf("Set content for an attribute\n");
            char clean_path[att-path];
            const char* delim = ".";
            strncpy(clean_path, path, att - path - 1);
            clean_path[att-path-1] = '\0';
            // printf("%s\n", clean_path);
            node_t* node = get_node_at_path(root, clean_path);
            if (node != NULL){
                char *attribute = strtok(att, delim);
                if (strcmp(attribute, "content") == 0){
                    printf("set content\n");
                    set_content(node, buf);
                    
                } else 
                    add_or_update_attribute(node, attribute, buf);
            }
            else
                printf("can't set!\n");
        }
    }

    
    return 0;
}


static int xml_mkdir(char *path, mode_t mode)
{
    node_t* node = insert_node_at_path(root, path);
    save_xml_file(root);

    return 0;
}

static int xml_rmdir(char *path, mode_t mode)
{
    node_t* node = get_node_at_path(root, path);
    delete_node(node);
    save_xml_file(root);

    return 0;
}

static int xml_chmod(char *path, mode_t mode)
{
    node_t* node = get_node_at_path(root, path);
    char modestr[6];
    sprintf (modestr, "%o", mode);
    add_or_update_attribute(node, "permissions", modestr);
    // save_xml_file(root);
    return 0;
}

static int xml_chown(char *path, uid_t u, gid_t g)
{
    node_t* node = get_node_at_path(root, path);
    char uidstr[6];
    sprintf (uidstr, "%d", u);
    char gidstr[6];
    sprintf (gidstr, "%d", g);
    add_or_update_attribute(node, "uid", uidstr);
    add_or_update_attribute(node, "gid", gidstr);
    // save_xml_file(root);
    return 0;
}

static int xml_truncate(char* path, off_t offset){
    return 0; //we never truncate
}

int xml_utime(const char *path, struct utimbuf *tb)
{
    time_t tim = time(NULL);
    char tim_str[12];
    sprintf (tim_str, "%d", tim);
    node_t* node = get_node_at_path(root, path);
    add_or_update_attribute(node, "modified", tim_str);
    // save_xml_file(root);
    return 0;
}

void xml_destroy()
{
    save_xml_file(root);
}




static struct fuse_operations xml_oper = {
    .getattr	= xml_getattr,
    .readdir	= xml_readdir,
    .open	    = xml_open,
    .read	    = xml_read,
    .mkdir      = xml_mkdir,
    .rmdir      = xml_rmdir,
    .write      = xml_write,
    .chmod      = xml_chmod,
    .chown      = xml_chown,
    .utime      = xml_utime,
    .truncate   = xml_truncate,
    .destroy    = xml_destroy
};

int main(int argc, char *argv[])
{
    save_initial_file_names(argv[argc-1]);
    // printf("%s\n", argv[argc]);
    root = open_file(argv[argc-1]);
    return fuse_main(argc-1, argv, &xml_oper);
}

