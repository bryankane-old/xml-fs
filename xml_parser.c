#include <stdio.h>
#include <string.h>
#include "libroxml/inc/roxml.h"
#include "xml_parser.h"

// returns the "file_name" attribute for a given node
char* get_file_name(node_t* node)
{
    node_t* attr_node = roxml_get_attr(node, file_name_attr, 0);
    if (attr_node == NULL)
        return "";
    return roxml_get_content(attr_node, NULL, 0, 0);
}

char* get_file_name_with_buffer(node_t* node, char* buffer)
{
    node_t* attr_node = roxml_get_attr(node, file_name_attr, 0);
    if (attr_node == NULL)
        return "";
    char* file_name = roxml_get_content(attr_node, NULL, 0, 0);
    printf("file_name: %s\n", file_name);
    strcpy(buffer, file_name);
    printf("buffer pointer: %p\n", &buffer);
    // buffer = roxml_get_content(attr_node, NULL, 0, 0);
}

// sets the "file_name" attribute for a given node
// (overwrites existing file names!)
// returns previous file name if it exists
char* set_file_name(node_t* node, char* file_name)
{
    return add_or_update_attribute(node, file_name_attr, file_name);
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

// Adds an empty child element with a given file name and parent
// (useful for initially creating a file before writing content, or "touch"ing a file)
node_t* add_child_element(node_t* parent, char* tag_name)
{
    return roxml_add_node(parent, 0, ROXML_ELM_NODE, tag_name, NULL);
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
    next_child_file_name = strtok(NULL, delim);
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

void get_all_child_file_names(char** buffer, node_t* parent)
{
    int num_children = roxml_get_chld_nb(parent);
    int i;
    printf("pointer to buffer: %p\n", buffer);
    for (i = 0; i < num_children; i++)
    {
        node_t* current_child = roxml_get_chld(parent, NULL, i);
        node_t* attr_node = roxml_get_attr(current_child, file_name_attr, 0);
        if (attr_node != NULL)
        {
            char* file_name = roxml_get_content(attr_node, NULL, 0, 0);
            buffer[i] =(char*)malloc(strlen(file_name));
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
    len = roxml_commit_changes(root, NULL, &buffer, 1);
    file_out = fopen(second_sample_file_name, "w");
    fwrite(buffer, 1, len, file_out);
    fclose(file_out);
}


// Open xml file at specified path, and return the root node
// if no file path specified, open the sample file
node_t* open_file(char* file_path)
{
    if (file_path == NULL)
        return roxml_load_doc(sample_file_name);
    else
        return roxml_load_doc(file_path);
}


// int main(void)
// {
//     char* path = "/";
//     node_t* root = open_file(sample_file_name);
//     node_t* current_dir = get_node_at_path(root, path);
//     int num_children = roxml_get_chld_nb(current_dir);
//     printf("%d\n", num_children);
//     char** children = get_all_child_file_names(current_dir);
//     int i;
//     for(i=0; i < num_children; i++){
//         printf("%s\nabc\n", children[i]);
//     }



//     // node_t * root = roxml_load_doc(sample_file_name);
//     // char* path = "/";
//     // node_t* ivory = get_node_at_path(root, path);
//     // get_all_child_file_names(ivory);


//     // // node_t* school = roxml_get_chld(root, NULL, 0);
//     // // print_all_children(school);
//     // // set_file_name(school, "school_0");
//     // save_xml_file(ivory);
//     // roxml_close(root);
//     return 0;
// }
