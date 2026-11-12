#include <stdio.h>
#include <assert.h>
#include "../include/bin_tree.h"

node_s*      bin_search_tree_find  (bin_tree_s* const bin_tree, bin_tree_elem elem);
error_code_e bin_search_tree_insert(bin_tree_s* const bin_tree, bin_tree_elem elem);

int main() {
    SAFE_CALL( log_start(), )

    bin_tree_s bin_search_tree = {};
    SAFE_CALL( bin_tree_assign(&bin_search_tree), )

    SAFE_CALL( bin_search_tree_insert(&bin_search_tree, 10), )
    SAFE_CALL( bin_search_tree_insert(&bin_search_tree, 20), )
    SAFE_CALL( bin_search_tree_insert(&bin_search_tree, 30), )
    SAFE_CALL( bin_search_tree_insert(&bin_search_tree, 15), )
    SAFE_CALL( bin_search_tree_insert(&bin_search_tree, 25), )
    SAFE_CALL( bin_search_tree_insert(&bin_search_tree, 5),  )

    printf(BNT_MDF, bin_search_tree_find(&bin_search_tree, 5)->data);

    SAFE_CALL( bin_tree_destroy(&bin_search_tree), )

    return 0;
}


error_code_e bin_search_tree_insert(bin_tree_s* const bin_tree, bin_tree_elem elem) {
    assert(bin_tree != NULL);

    node_s** node = &bin_tree->root;

    for (ssize_t cnt = 0; cnt <= bin_tree->count_nodes; cnt++) {
        if ((*node) == NULL) {
            bin_tree_insert(bin_tree, node, elem);
            LOG_OK("bin_search_tree_insert successfully");
            return SUCCESS;
        } else if (elem < (*node)->data) {
            node = &(*node)->left;
        } else if (elem > (*node)->data) {
            node = &(*node)->right;
        }
    }

    LOG_ERROR("infinite cycle");
    return INCORRECT_PARAM;
}


node_s* bin_search_tree_find(bin_tree_s* const bin_tree, bin_tree_elem elem) {
    assert(bin_tree != NULL);

    node_s** node = &bin_tree->root;

    for (ssize_t cnt = 0; cnt < bin_tree->count_nodes; cnt++) {
        if((*node)->data == elem) {
            return *node;
        } if ((*node)->left != NULL && elem < (*node)->data) {
            node = &(*node)->left;
        } else if ((*node)->right != NULL && elem > (*node)->data) {
            node = &(*node)->right;
        }
    }

    return NULL;
}
