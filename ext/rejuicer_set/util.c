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
// output Array object from internal set
//
void to_array(root_node root, VALUE array)
{
    int i, count;

    for(i = 0, count = 0; i < ROOT_NODE_SIZE || count < root->children_size; i++) {
        if (root->index[i]) {
            search_and_get_array((branch_node)root->index[i], array);
            count++;
        }
    }
}

void search_and_get_array(branch_node branch, VALUE array)
{
    int i, count;

    if (branch->level == LAST_BRANCH_LEVEL) {
        for(i = 0, count = 0; i < BRANCH_NODE_SIZE || count < branch->children_size; i++) {
            if (branch->index[i]) {
                search_and_get_array_at_leaf((leaf_node)branch->index[i], array);
                count++;
            }
        }
    } else {
        for(i = 0, count = 0; i < BRANCH_NODE_SIZE || count < branch->children_size; i++) {
            if (branch->index[i]) {
                search_and_get_array((branch_node)branch->index[i], array);
                count++;
            }
        }
    }
}

void search_and_get_array_at_leaf(leaf_node leaf, VALUE array)
{
    int i = 0;
    unsigned int x;

    x = leaf->data;

    while(x) {
        if (x & 1) {
            rb_ary_push(array, UINT2NUM(leaf->num * INDEX_PER_SIZE[LAST_BRANCH_LEVEL] + i));
        }
        x = x >> 1;
        i++;
    }
}


//
// bit calc
//
unsigned int bit_count(unsigned int x)
{
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0f0f0f0f;
    x = x + (x >> 8);
    x = x + (x >> 16);
    return x & 0x0000003F;
}
