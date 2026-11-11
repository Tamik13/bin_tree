#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "../include/bin_tree.h"
#include "../include/stack.h"


error_code_e bin_tree_assign (bin_tree_s* const bin_tree) {
    assert(bin_tree != NULL);

    bin_tree->count_nodes = 0;
    bin_tree->root        = NULL;

    LOG_OK("bin_tree_assign successfully");
    return SUCCESS;
}


error_code_e bin_tree_destroy(bin_tree_s* const bin_tree) {
    assert(bin_tree != NULL);

    if (bin_tree->root == NULL) {
        LOG_OK("bin_tree_dfs successfully");
        return SUCCESS;
    }

    ssize_t count_nodes = bin_tree->count_nodes;
    ON_DBG(ssize_t  cnt = 0;)
    node_s** node       = NULL;
    stack_s  stack      = {};
    STACK_INIT(stack, count_nodes);
    SAFE_CALL( stack_push(&stack, &bin_tree->root), stack_destroy(&stack));

    while (stack.size > 0) {
        ON_DBG(
        if (cnt++ > count_nodes * 3) {
            stack_destroy(&stack);
            LOG_ERROR("infinite cycle");
            return INCORRECT_SIZE;
        }
        )

        SAFE_CALL( stack_pop(&stack, &node), stack_destroy(&stack));

        if ((*node)->left != NULL && (*node)->right != NULL) {
            SAFE_CALL( stack_push(&stack, node),            stack_destroy(&stack))
            SAFE_CALL( stack_push(&stack, &(*node)->left),  stack_destroy(&stack))
            SAFE_CALL( stack_push(&stack, &(*node)->right), stack_destroy(&stack))
        } else if ((*node)->left != NULL) {
            SAFE_CALL( stack_push(&stack, node),            stack_destroy(&stack))
            SAFE_CALL( stack_push(&stack, &(*node)->left),  stack_destroy(&stack))
        } else if ((*node)->right != NULL) {
            SAFE_CALL( stack_push(&stack, node),            stack_destroy(&stack))
            SAFE_CALL( stack_push(&stack, &(*node)->right), stack_destroy(&stack))
        } else {
            SAFE_CALL( bin_tree_del(bin_tree, node), )
        }
    }

    SAFE_CALL(stack_destroy(&stack), );
    LOG_OK("bin_tree_dfs successfully");
    return SUCCESS;
}


error_code_e bin_tree_insert(bin_tree_s* const bin_tree, node_s** const node, bin_tree_elem elem) {

    *node = (node_s*)calloc(1, sizeof(node_s));
    (*node)->left  = NULL;
    (*node)->right = NULL;
    (*node)->data  = elem;
    bin_tree->count_nodes++;

    LOG_OK("bin_tree_insert successfully elem = " BNT_MDF, elem);
    return SUCCESS;
}


error_code_e bin_tree_del(bin_tree_s* const bin_tree, node_s** const node) {
    assert(node != NULL);
    assert(bin_tree->count_nodes > 0);

    free(*node);
    *node = NULL;
    bin_tree->count_nodes--;

    LOG_OK("bin_tree_del successfully");
    return SUCCESS;
}


error_code_e bin_tree_verify (const bin_tree_s* const bin_tree) {
    return SUCCESS;
}


error_code_e bin_tree_print_postorder(bin_tree_s* const bin_tree, char** buffer) {
    assert(bin_tree != NULL);

    ssize_t count_nodes = bin_tree->count_nodes;
    *buffer             = (char*)calloc((size_t)count_nodes * 10, sizeof(char));
    ssize_t count_write = 0;

    if (*buffer == NULL) {
        LOG_ERROR("allocation error");
        return ALLOCATION_ERROR;
    }

    ON_DBG(ssize_t cnt  = 0;)
    node_s**   node     = NULL;
    stack_s    stack    = {};
    STACK_INIT(stack, bin_tree->count_nodes);
    SAFE_CALL( stack_push(&stack, &bin_tree->root), stack_destroy(&stack));

    node_s  void_node     = {};
    node_s* void_node_ptr = &void_node;

    while (stack.size > 0) {
        ON_DBG(
        if (cnt++ > bin_tree->count_nodes * 3) {
            LOG_ERROR("infinite cycle");
            return INCORRECT_SIZE;
        }
        )

        SAFE_CALL( stack_pop(&stack, &node), );

        if (node != &void_node_ptr) {
            count_write += snprintf(*buffer + count_write, 10, "(" BNT_MDF, (*node)->data);
        }

        if ((*node)->left != NULL && (*node)->right != NULL) {
            SAFE_CALL( stack_push(&stack, &void_node_ptr),  stack_destroy(&stack))
            SAFE_CALL( stack_push(&stack, &(*node)->left),  stack_destroy(&stack))
            SAFE_CALL( stack_push(&stack, &(*node)->right), stack_destroy(&stack))
        } else if ((*node)->left != NULL) {
            SAFE_CALL( stack_push(&stack, &void_node_ptr),  stack_destroy(&stack))
            SAFE_CALL( stack_push(&stack, &(*node)->left),  stack_destroy(&stack))
        } else if ((*node)->right != NULL) {
            SAFE_CALL( stack_push(&stack, &void_node_ptr),  stack_destroy(&stack))
            SAFE_CALL( stack_push(&stack, &(*node)->right), stack_destroy(&stack))
        } else {
            count_write += snprintf(*buffer + count_write, 2, ")");
        }
    }

    SAFE_CALL(stack_destroy(&stack), );

    LOG_OK("stack_push successfully");
    return SUCCESS;
}


void bin_tree_dmp(bin_tree_s* const bin_tree) {
    assert(bin_tree != NULL);

    char* buffer;
    bin_tree_print_postorder(bin_tree, &buffer);

    if (bin_tree_img(bin_tree) != SUCCESS) {
        LOG_ERROR("cant generate bin_tree img");
        return;
    }

    LOG("bin_tree %p {",               bin_tree);
    LOG("bin_tree->count_nodes = %zd", bin_tree->count_nodes);
    LOG("POSTORDER: %s", buffer);
    LOG("}");

    free(buffer);
    buffer = NULL;

    size_t blksize = 0;
    if (stat_get_blksize("./logs/dot/graph.svg", &blksize) != SUCCESS) {
        LOG_ERROR("STAT ERROR errno: %s", strerror(errno));
        return;
    }

    buffer = (char*)calloc(blksize, sizeof(char));

    int dot_file = open("./logs/dot/graph.svg", O_RDONLY);

    if (dot_file == -1) {
        LOG_ERROR("cant open dot file");
        return;
    }

    ssize_t count_read = 0;
    while ((count_read = read(dot_file, buffer, blksize)) > 0) {
        ssize_t new_ind = 0;

        for (ssize_t true_ind = 0; true_ind < count_read; true_ind++) {

            if (buffer[true_ind] != '\n') {
                buffer[new_ind] = buffer[true_ind];
                new_ind++;
            }
            if (true_ind >= 4 && strncmp(&buffer[true_ind - 4], "/svg>", 5) == 0) {
                break;

            }
        }

        fwrite(buffer, 1, (size_t)new_ind,  GLOBAL_LOG_FILE);
    }

    fprintf(GLOBAL_LOG_FILE, "\n");

    if (close(dot_file) == -1) {
        LOG_ERROR("CANT CLOSE dot_file");
        return;
    }


    LOG_OK("bin_tree_dmp successfully");
    return;
}


error_code_e bin_tree_img(bin_tree_s* const bin_tree) {
    assert(bin_tree != NULL);

    const char* const file_name = "./logs/dot/graph.dot";

    FILE* dot_file = fopen(file_name, "w");
    if (dot_file == NULL) {
        LOG_ERROR("cant open dot file");
        return ERROR_DURING_OPEN;
    }

    fprintf(dot_file,   "digraph {\n"
                            "\tbgcolor=\"" BGCOLOR_HX "\";\n"
                            "\tnodesep=0.6\n"
                            "\trank_sep=1.0\n"
                            );

    ON_DBG(    ssize_t cnt         = 0;)
               node_s**   node     = NULL;
               stack_s    stack    = {};
               STACK_INIT(stack, bin_tree->count_nodes);
    SAFE_CALL( stack_push(&stack, &bin_tree->root), stack_destroy(&stack));

    while (stack.size > 0) {
        ON_DBG(
        if (cnt++ > bin_tree->count_nodes * 3) {
            LOG_ERROR("infinite cycle");
            return INCORRECT_SIZE;
        }
        )

        SAFE_CALL( stack_pop(&stack, &node), stack_destroy(&stack));
        fprintf(dot_file, "\tnode" BNT_MDF "[style=filled; fillcolor=\"" AQUA_HX "\" label = " BNT_MDF "]\n", (*node)->data, (*node)->data);

        if ((*node)->left != NULL && (*node)->right != NULL) {
            fprintf(dot_file, "\tnode" BNT_MDF " -> node" BNT_MDF "[color=\"" GREEN_HX "\"]\n", (*node)->data, (*node)->left->data);
            fprintf(dot_file, "\tnode" BNT_MDF " -> node" BNT_MDF "[color=\"" RED_HX   "\"]\n", (*node)->data, (*node)->right->data);
            SAFE_CALL( stack_push(&stack, &(*node)->left),  stack_destroy(&stack))
            SAFE_CALL( stack_push(&stack, &(*node)->right), stack_destroy(&stack))
        } else if ((*node)->left != NULL) {
            fprintf(dot_file, "\tnode" BNT_MDF " -> node" BNT_MDF "[color=\"" GREEN_HX "\"]\n", (*node)->data, (*node)->left->data);
            SAFE_CALL( stack_push(&stack, &(*node)->left),  stack_destroy(&stack))
        } else if ((*node)->right != NULL) {
            fprintf(dot_file, "\tnode" BNT_MDF " -> node" BNT_MDF "[color=\"" RED_HX   "\"]\n", (*node)->data, (*node)->right->data);
            SAFE_CALL( stack_push(&stack, &(*node)->right), stack_destroy(&stack))
        }
    }

    fprintf(dot_file, "}");

    if (fclose(dot_file) == EOF) {
        LOG_ERROR("cant close dot file");
        return ERROR_DURING_CLOSE;
    }

    int status = system("dot -Tsvg ./logs/dot/graph.dot -o ./logs/dot/graph.svg");

    if (status == -1) {
        LOG_ERROR("status of system() = -1");
        return SYSTEM_ERROR;
    }

    SAFE_CALL(stack_destroy(&stack), );

    LOG_OK("graph graphic dmp successfully");
    return SUCCESS;
}
