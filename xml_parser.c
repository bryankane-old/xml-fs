#include <stdio.h>
#include "libroxml/inc/roxml.h"
#include "xml_parser.h"

char* get_file_name(node_t* node)
{
    node_t* attr_node = roxml_get_attr(node, file_name_attr, 0);
    if (attr_node == NULL)
        return "";
    return roxml_get_content(attr_node, NULL, 0, 0);
}

char* set_file_name(node_t* node, char* file_name)
{
    return add_or_update_attribute(node, file_name_attr, file_name);
}

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


int main(void)
{
    node_t * root = roxml_load_doc(sample_file_name);
    node_t* school = roxml_get_chld(root, NULL, 0);
    print_all_children(school);
    set_file_name(school, "school_0");
    save_xml_file(root);
    roxml_close(root);
    return 0;
}
