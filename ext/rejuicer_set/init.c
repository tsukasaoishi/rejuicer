//************************************
//         rejuicer.c
//
//       Tsukasa OISHI
//
//         2010/03/17
//************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ruby.h>
#include "rejuicer_set.h"

//
// initialize
//
void init_root_node(root_node root)
{
    int i;

    for(i = 0; i < ROOT_NODE_SIZE; i++) {
        root->index[i] = (void*)NULL;
    }
    root->size = 0;
    root->children_size = 0;
}

void *init_branch_node(int level, unsigned int value)
{
    return init_branch_node2(level, (value / INDEX_PER_SIZE[level - 1]));
}

void *init_branch_node2(int level, unsigned int num)
{
    int i;
    branch_node branch;

    if (!(branch = (branch_node)malloc(sizeof(struct _branch_node)))) {
        rb_raise(rb_eStandardError, "memory is not enough");
    }

    for(i = 0; i < BRANCH_NODE_SIZE; i++) {
        branch->index[i] = (void*)NULL;
    }

    branch->level = level;
    branch->num = num;
    branch->children_size = 0;

    return (void*)branch;
}

void *init_leaf_node(unsigned int value)
{
    return init_leaf_node2(value / INDEX_PER_SIZE[LAST_BRANCH_LEVEL]);
}

void *init_leaf_node2(unsigned int num)
{
    leaf_node leaf;

    if(!(leaf = (leaf_node)malloc(sizeof(struct _leaf_node)))) {
        rb_raise(rb_eStandardError, "memory is not enough");
    }

    leaf->num = num;
    leaf->data = 0;

    return (void*)leaf;
}

//
// initialize copy
//
void copy_root_node(root_node root, root_node orig)
{
    int i, count;

    root->children_size = orig->children_size;

    for (i = 0, count = 0; i < ROOT_NODE_SIZE || count < orig->children_size; i++) {
        if (orig->index[i]) {
            root->index[i] = (branch_node)init_and_copy_brance_node(root, (branch_node)orig->index[i]);
            count++;
        }
    }
}

void copy_branch_node(root_node root, branch_node branch, branch_node orig)
{
    int i, count;

    branch->num = orig->num;
    branch->level = orig->level;
    branch->children_size = orig->children_size;

    if (orig->level == LAST_BRANCH_LEVEL) {
        for(i = 0, count = 0; i < BRANCH_NODE_SIZE || count < orig->children_size; i++) {
            if (orig->index[i]) {
                branch->index[i] = (leaf_node)init_and_copy_leaf_node(root, (leaf_node)orig->index[i]);
                count++;
            }
        }
    } else {
        for(i = 0, count = 0; i < BRANCH_NODE_SIZE || count < orig->children_size; i++) {
            if (orig->index[i]) {
                branch->index[i] = (branch_node)init_and_copy_brance_node(root, (branch_node)orig->index[i]);
                count++;
            }
        }
    }
}

void *init_and_copy_brance_node(root_node root, branch_node orig)
{
    branch_node ret;

    ret = init_branch_node2(orig->level, orig->num);
    copy_branch_node(root, ret, orig);

    return ret;
}

void *init_and_copy_leaf_node(root_node root, leaf_node orig)
{
    leaf_node ret;

    ret = init_leaf_node2(orig->num);
    ret->data = orig->data;
    root->size += bit_count(ret->data);

    return ret;
}

//
// memory free
//
void destroy_all(root_node root)
{
    destroy_all_branches(root);
    free(root);
}

void destroy_all_branches(root_node root)
{
    int i, count;
    
    for(i = 0, count = 0; i < ROOT_NODE_SIZE || count < root->children_size; i++) {
        if (root->index[i]) {
            destroy_branch((branch_node)root->index[i]);
            root->index[i] = NULL;
            count++;
        }
    }

    root->size = 0;
    root->children_size = 0;
}

void destroy_branch(branch_node branch)
{
    int i, count;

    if (branch->level == LAST_BRANCH_LEVEL) {
        for(i = 0, count = 0; i < BRANCH_NODE_SIZE || count < branch->children_size; i++) {
            if (branch->index[i]) {
                free((leaf_node)branch->index[i]);
                branch->index[i] = NULL;
                count++;
            }
        }
    } else {
        for(i = 0, count = 0; i < BRANCH_NODE_SIZE || count < branch->children_size; i++) {
            if (branch->index[i]) {
                destroy_branch((branch_node)branch->index[i]);
                branch->index[i] = NULL;
                count++;
            }
        }
    }
    free(branch);
}
