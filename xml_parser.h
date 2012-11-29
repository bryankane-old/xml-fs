#define file_name_attr "file_name"

#define sample_file_name "sample.xml"
#define second_sample_file_name "sample_2.xml"

char* get_file_name(node_t* node);
char* set_file_name(node_t* node, char* file_name);
char* add_or_update_attribute(node_t* node, char* key, char* value);
node_t* add_child_element(node_t* parent, char* tag_name);
node_t* get_child_by_file_name(node_t* parent, char* file_name);
node_t* get_node_at_path(node_t* root, char* path);
void print_all_children(node_t* node);
void save_xml_file(node_t* root);
