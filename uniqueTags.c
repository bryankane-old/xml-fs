#include "list.h"
#include <stdio.h>
#include <string.h>
#include "xml_parser.h"

void createUniqueTagsRecursive(node_t *node){
    //Create linked list to temporarily store 
    int num; //The number to be concatenated to the original tag (allocated later)
    struct linked_list * ll = ll_create();
    int numChildren = roxml_get_chld_nb(node);
    int i;
    for(i=0; i<numChildren; i++){
        //if this tag already exists in the the linked list
        node_t* child = roxml_get_chld(node, NULL, i);
        char* tagName = roxml_get_name(child, NULL, 0);

        if(ll_contains(ll, tagName) != NULL){
            num = ll_plusplus(ll, tagName);  //must subtract by one because our indexing starts at 0 not 1
        }
        else{
            ll_add(ll, tagName);
            num = 0;
        }
        char newTagName[1000];
        snprintf(newTagName, sizeof(tagName) + 100, "%s_%d", tagName, num);
        set_file_name(child, newTagName);
        add_attribute_if_doesnt_exist(child, "permissions", "40755");
        add_attribute_if_doesnt_exist(child, "uid", "0");
        add_attribute_if_doesnt_exist(child, "gid", "0");
        time_t tim = time(NULL);
        char tim_str[12];
        sprintf (tim_str, "%d", tim);
        add_attribute_if_doesnt_exist(child, "modified", tim_str);
        createUniqueTagsRecursive(child);
    }
    ll_destroy(ll);
}

//Used for newly written nodes
char* newNodeUniqueTag(node_t *node, char* newTag){
 //Create linked list to temporarily store 
    int count=0; //The number to be concatenated to the original tag (allocated later)
    int numChildren = roxml_get_chld_nb(node);
    int i;
    for(i=0; i<numChildren; i++){          
        node_t* child = roxml_get_chld(node, NULL, i);
        char* tagName = roxml_get_name(child, NULL, 0);
         if(strcmp(newTag, tagName) == 0){
            count++;
         }
    }
    char file_name[1000];
    snprintf(file_name, sizeof(newTag) + 100, "%s_%d", newTag, count);
    return file_name;
}
