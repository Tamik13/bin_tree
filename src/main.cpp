#include <stdio.h>
#include "../include/bin_tree.h"

int main() {
    SAFE_CALL( log_start(), )

    bin_tree_s tree = {};
    SAFE_CALL( bin_tree_assign(&tree), )

    SAFE_CALL( bin_tree_insert(&tree, &tree.root, 0),                   )
    SAFE_CALL( bin_tree_insert(&tree, &tree.root->left, 1),             )

    SAFE_CALL( bin_tree_insert(&tree, &tree.root->left->left, 2),       )
    SAFE_CALL( bin_tree_insert(&tree, &tree.root->left->right, 3),      )
    SAFE_CALL( bin_tree_insert(&tree, &tree.root->left->left->left, 4), )
               bin_tree_dmp(&tree);
    SAFE_CALL( bin_tree_destroy(&tree),                                 )

    return 0;
}
