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
// join
//
void join(root_node ret_set, root_node set0, root_node set1)
{
    int i;

    if (set0->size == 0 && set1->size == 0) {
        return;
    } else if (set0->size == 0) {
        copy_root_node(ret_set, set1);
        return;
    } else if (set1->size == 0) {
        copy_root_node(ret_set, set0);
        return;
    }

    for (i = 0; i < ROOT_NODE_SIZE; i++) {
        if (set0->index[i] && set1->index[i]) {
            ret_set->index[i] = (branch_node)init_and_join_brance_node(ret_set, (branch_node)set0->index[i], (branch_node)set1->index[i]);
        } else if(set0->index[i]) {
            ret_set->index[i] = (branch_node)init_and_copy_brance_node(ret_set, (branch_node)set0->index[i]);
        } else if (set1->index[i]) {
            ret_set->index[i] = (branch_node)init_and_copy_brance_node(ret_set, (branch_node)set1->index[i]);
        }
    }
}


void join_branch_node(root_node root, branch_node ret_set, branch_node set0, branch_node set1)
{
    int i;

    if (ret_set->level == LAST_BRANCH_LEVEL) {
        last_join_branch_node(root, ret_set, set0, set1);
    } else {
        middle_join_branch_node(root, ret_set, set0, set1);
    }
}

void middle_join_branch_node(root_node root, branch_node ret_set, branch_node set0, branch_node set1)
{
    int i;

    for (i = 0; i < BRANCH_NODE_SIZE; i++) {
        if (set0->index[i] && set1->index[i]) {
            ret_set->index[i] = (branch_node)init_and_join_brance_node(root, (branch_node)set0->index[i], (branch_node)set1->index[i]);
        } else if (set0->index[i]) {
            ret_set->index[i] = (branch_node)init_and_copy_brance_node(root, (branch_node)set0->index[i]);
        } else if (set1->index[i]) {
            ret_set->index[i] = (branch_node)init_and_copy_brance_node(root, (branch_node)set1->index[i]);
        }
    }
}

void last_join_branch_node(root_node root, branch_node ret_set, branch_node set0, branch_node set1)
{
    int i;

    for (i = 0; i < BRANCH_NODE_SIZE; i++) {
        if (set0->index[i] && set1->index[i]) {
            ret_set->index[i] = (leaf_node)init_and_join_leaf_node(root, (leaf_node)set0->index[i], (leaf_node)set1->index[i]);
        } else if (set0->index[i]) {
            ret_set->index[i] = (leaf_node)init_and_copy_leaf_node(root, (leaf_node)set0->index[i]);
        } else if (set1->index[i]) {
            ret_set->index[i] = (leaf_node)init_and_copy_leaf_node(root, (leaf_node)set1->index[i]);
        }
    }
}

void join_leaf_node(root_node root, leaf_node ret_set, leaf_node set0, leaf_node set1)
{
    ret_set->data = set0->data | set1->data;
    root->size += bit_count(ret_set->data);
}

void *init_and_join_brance_node(root_node root, branch_node set0, branch_node set1)
{
    branch_node ret;

    ret = init_branch_node2(set0->level, set0->num);
    join_branch_node(root, ret, set0, set1);

    return ret;
}

void *init_and_join_leaf_node(root_node root, leaf_node set0, leaf_node set1)
{
    leaf_node ret;

    ret = init_leaf_node2(set0->num);
    join_leaf_node(root, ret, set0, set1);

    return ret;
}
