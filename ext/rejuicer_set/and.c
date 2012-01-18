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
// intersection
//
void intersection(root_node ret_set, root_node set0, root_node set1)
{
    int i, count;
    root_node base, other;

    if (set0->size == 0 || set1->size == 0) {
        return;
    } else if (set0->size > set1->size) {
        base = set1;
        other = set0;
    } else {
        base = set0;
        other = set1;
    }

    for (i = 0, count = 0; i < ROOT_NODE_SIZE || count < base->children_size; i++) {
        if (base->index[i]) {
            count++;
            if (other->index[i]) {
                ret_set->index[i] = (branch_node)init_and_intersection_branch_node(ret_set, (branch_node)base->index[i], (branch_node)other->index[i]);
            }
        }
    }
}

void intersection_branch_node(root_node root, branch_node ret_set, branch_node base, branch_node other)
{
    if (ret_set->level == LAST_BRANCH_LEVEL) {
        last_intersection_branch_node(root, ret_set, base, other);
    } else {
        middel_intersection_branch_node(root, ret_set, base, other);
    }
}

void middel_intersection_branch_node(root_node root, branch_node ret_set, branch_node base, branch_node other)
{
    int i, count;

    for (i = 0, count = 0; i < BRANCH_NODE_SIZE || count < base->children_size; i++) {
        if (base->index[i]) {
            count++;
            if (other->index[i]) {
                ret_set->index[i] = (branch_node)init_and_intersection_branch_node(root, (branch_node)base->index[i], (branch_node)other->index[i]);
            }
        }
    }
}

void last_intersection_branch_node(root_node root, branch_node ret_set, branch_node base, branch_node other)
{
    int i, count;

    for (i = 0, count = 0; i < BRANCH_NODE_SIZE || count < base->children_size; i++) {
        if (base->index[i]) {
            count++;
            if (other->index[i]) {
                ret_set->index[i] = init_leaf_node2(((leaf_node)base->index[i])->num);
                intersection_leaf_node(root, (leaf_node)ret_set->index[i], (leaf_node)base->index[i], (leaf_node)other->index[i]);
            }
        }
    }
}


void intersection_leaf_node(root_node root, leaf_node ret_set, leaf_node base, leaf_node other)
{
    ret_set->data = base->data & other->data;
    root->size += bit_count(ret_set->data);
}


void *init_and_intersection_branch_node(root_node root, branch_node base, branch_node other)
{
    branch_node ret;

    ret = init_branch_node2(base->level, base->num);
    intersection_branch_node(root, ret, base, other);

    return ret;
}
