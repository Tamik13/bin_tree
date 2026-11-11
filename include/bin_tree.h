#ifndef _BIN_TREE
#define _BIN_TREE

#include "../include/debugging.h"

typedef double bin_tree_elem;
#define BNT_MDF "%.5g"

struct node_s {
    bin_tree_elem data;
    node_s* left;
    node_s* right;
};

struct bin_tree_s {
    node_s*       root;
    ssize_t       count_nodes;
};

error_code_e bin_tree_assign (      bin_tree_s* const bin_tree);
error_code_e bin_tree_destroy(      bin_tree_s* const bin_tree);
error_code_e bin_tree_insert (      bin_tree_s* const bin_tree, node_s** const node, bin_tree_elem elem);
error_code_e bin_tree_del    (      bin_tree_s* const bin_tree, node_s** const node);
error_code_e bin_tree_verify (const bin_tree_s* const bin_tree);

void bin_tree_dmp                    (bin_tree_s* const bin_tree);
error_code_e bin_tree_img            (bin_tree_s* const bin_tree);
error_code_e bin_tree_print_postorder(bin_tree_s* const bin_tree, char** buffer);

#endif
