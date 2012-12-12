#include <stdio.h>
#include <string.h>
#include "libroxml/inc/roxml.h"
#include "xml_parser.h"
#include "uniqueTags.h"

char* path;

// returns the "file_name" attribute for a given node
char* get_file_name(node_t* node)
{
    node_t* attr_node = roxml_get_attr(node, file_name_attr, 0);
    if (attr_node == NULL)
        return "";
    return roxml_get_content(attr_node, NULL, 0, 0);
}

// sets the "file_name" attribute for a given node
// (overwrites existing file names!)
// returns previous file name if it exists
char* set_file_name(node_t* node, char* file_name)
{
    return add_or_update_attribute(node, file_name_attr, file_name);
}

// returns -1 if the node is a leaf (has no children)
// or 0 if it is not a leaf (has at least one child)
int is_leaf (node_t* root, char* path)
{
    node_t* node = get_node_at_path(root, path);
    if (roxml_get_chld_nb(node) == 0)
    {
        return -1;
    }
    return 0;
}

// to list attributes:
//
// char** attr = get_all_attributes(current_dir);
// int num_attributes = roxml_get_attr_nb(current_dir);
// for (i = 0; i < num_attributes; i++)
//     printf("%s\n", attr[0]);

// returns an array of attribute names for a given node
char** get_all_attributes(node_t* node)
{
    int num_attributes = roxml_get_attr_nb(node);
    int i;
    char** attributes = malloc(sizeof(char*)*(num_attributes));
    for (i = 0; i < num_attributes; i++)
    {
        node_t* current_attr = roxml_get_attr(node, NULL, i);
        attributes[i] = malloc(sizeof(char) * 255);
        roxml_get_name(current_attr, attributes[i], 255);
    }
    return attributes;
}

//get attribute content:
char* get_attr_content(node_t* node, char* name)
{
    char* content;
    node_t* current_attr = roxml_get_attr(node, name, 0);
    if (current_attr != NULL)
        content = roxml_get_content(current_attr, NULL, 0, 0);
    else 
        content = "Not an attribute!";
    return content;
}

void set_content(node_t* node, char* content) {
    node_t* prev_txt = roxml_get_txt(node, 0);
    if (prev_txt != NULL)
    {
        roxml_del_node(prev_txt);
    }
    roxml_add_node(node, 0, ROXML_TXT_NODE, NULL, content);
    return;
}

// adds in an attribute for a given node, key (attribute name), and value
// overwrites attribute value if exists (returns old value)
char* add_or_update_attribute(node_t* node, char* key, char* value)
{
    node_t* prev_attr = roxml_get_attr(node, key, 0);
    char* prev_value = NULL;
    if (prev_attr != NULL)
    {
        prev_value = roxml_get_content(prev_attr, NULL, 0, 0);
        roxml_del_node(prev_attr);
    }
    roxml_add_node(node, 0, ROXML_ATTR_NODE, key, value);
    return prev_value;
}

// adds in an attribute for a given node, key (attribute name), and value
// overwrites attribute value if exists (returns old value)
char* add_attribute_if_doesnt_exist(node_t* node, char* key, char* value)
{
    node_t* prev_attr = roxml_get_attr(node, key, 0);
    char* prev_value = NULL;
    if (prev_attr == NULL)
    {
        roxml_add_node(node, 0, ROXML_ATTR_NODE, key, value);
    }
    return prev_value;
}

// Adds an empty child element with a given file name and parent
// (useful for initially creating a file before writing content, or "touch"ing a file)
node_t* add_child_element(node_t* parent, char* tag_name)
{
    char* file_name = newNodeUniqueTag(parent, tag_name);
    node_t* node = roxml_add_node(parent, 0, ROXML_ELM_NODE, tag_name, NULL);
    add_or_update_attribute(node, "file_name", file_name);
    add_or_update_attribute(node, "permissions", "40755");
    add_or_update_attribute(node, "uid", "0");
    add_or_update_attribute(node, "gid", "0");
    return node;
}

// Finds the child of a parent element with a specific file name
// or returns NULL if it doesn't exist.
node_t* get_child_by_file_name(node_t* parent, char* file_name)
{
    int num_children = roxml_get_chld_nb(parent);
    int i;
    for (i = 0; i < num_children; i++)
    {
        node_t* current_child = roxml_get_chld(parent, NULL, i);

        // since file_names are always going to be unique,
        // as soon as we find a match, we can return it.
        if (strcmp(get_file_name(current_child), file_name) == 0)
            return current_child;
    }
    return NULL;
}

// Given a string path of file names (delimited by a backslash),
// returns the node at that path, or NULL if there's a problem finding it
node_t* get_node_at_path(node_t* root, char* path)
{
    char current_path[strlen(path)];
    strcpy(current_path, path);
    const char* delim = "/";
    char* next_child_file_name = strtok(current_path, delim);
    // next_child_file_name = strtok(NULL, delim);
    node_t* node = root;
    while (next_child_file_name != NULL)
    {
        node_t* next_child = get_child_by_file_name(node, next_child_file_name);
        if (next_child == NULL)
            return NULL;
        node = next_child;
        next_child_file_name = strtok(NULL, delim);
    }
    return node;
}

// Given a string path of file names (delimited by a backslash),
// returns the node at that path, or NULL if there's a problem finding it
node_t* insert_node_at_path(node_t* root, char* path)
{
    char current_path[strlen(path)];
    strcpy(current_path, path);
    const char* delim = "/";
    char* next_child_file_name = strtok(current_path, delim);
    // next_child_file_name = strtok(NULL, delim);
    node_t* node = root;
    while (next_child_file_name != NULL)
    {
        node_t* next_child = get_child_by_file_name(node, next_child_file_name);
        if (next_child == NULL) {
            printf("Inserting node: %s\n", next_child_file_name);
            add_child_element(node, next_child_file_name);
            return NULL;
        }
        node = next_child;
        next_child_file_name = strtok(NULL, delim);
    }
    return node;
}

void delete_node(node_t* node)
{
    roxml_del_node(node);
}

void get_all_child_file_names(char** buffer, node_t* parent)
{
    int num_children = roxml_get_chld_nb(parent);
    int i;
    for (i = 0; i < num_children; i++)
    {
        node_t* current_child = roxml_get_chld(parent, NULL, i);
        node_t* attr_node = roxml_get_attr(current_child, file_name_attr, 0);
        if (attr_node != NULL)
        {
            char* file_name = roxml_get_content(attr_node, NULL, 0, 0);
            buffer[i] =(char*)malloc((strlen(file_name)+1) * sizeof(char));
            strcpy(buffer[i], file_name);
        }
    }
}

// iterates through all children for a given node, and prints out their file name
void print_all_children(node_t* node)
{
    int num_children = roxml_get_chld_nb(node);
    int i;
    for (i = 0; i < num_children; i++)
    {
        node_t* current_child = roxml_get_chld(node, NULL, i);
        printf("%s\n", get_file_name(current_child));
    }
}

// opens up the xml file and writes temp xml
// takes in the file's root (should be open already)
void save_xml_file(node_t* root)
{
    int len = 0;
    char * buffer = NULL;
    FILE * file_out;
    len = roxml_commit_changes(root, NULL, &buffer, 0);
    file_out = fopen(path, "w");
    fwrite(buffer, 1, len, file_out);
    fclose(file_out);
}


// Open xml file at specified path, and return the root node
// if no file path specified, open the sample file
node_t* open_file(char* file_path)
{
    path = file_path;
    if (path == NULL)
        return roxml_load_doc(sample_file_name);
    else
        return roxml_load_doc(file_path);
}

void save_initial_file_names(char* file_path)
{
    node_t* root = open_file(file_path);
    createUniqueTagsRecursive(root);
    save_xml_file(root);
    roxml_close(root);
}


// int main(void)
// {
// //     char* path = "/";
//     node_t* root = open_file(sample_file_name);
//     createUniqueTagsRecursive(root);
// //     node_t* current_dir = get_node_at_path(root, path);
// //     int num_children = roxml_get_chld_nb(current_dir);
// //     printf("%d\n", num_children);
// //     char** children = get_all_child_file_names(current_dir);
// //     int i;
// //     for(i=0; i < num_children; i++){
// //         printf("%s\nabc\n", children[i]);
// //     }



// //     // node_t * root = roxml_load_doc(sample_file_name);
// //     // char* path = "/";
// //     // node_t* ivory = get_node_at_path(root, path);
// //     // get_all_child_file_names(ivory);


// //     // // node_t* school = roxml_get_chld(root, NULL, 0);
// //     // // print_all_children(school);
// //     // // set_file_name(school, "school_0");
//     save_xml_file(root);
//     roxml_close(root);
//     return 0;
// }
