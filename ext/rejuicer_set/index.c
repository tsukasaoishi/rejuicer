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
// insert element into set
//
void add_num(root_node root, unsigned int value)
{
    unsigned int quotient, remainder;

    quotient = value / INDEX_PER_SIZE[0];
    remainder = value % INDEX_PER_SIZE[0];

    if(!(root->index[quotient])) {
        root->index[quotient] = init_branch_node(1, quotient);
        root->children_size++;
    }

    if (search_and_insert((branch_node)root->index[quotient], 1, remainder, value)) {
        root->size++;
    }
}

int search_and_insert(branch_node branch, int level, unsigned int value, unsigned int original)
{
    unsigned int quotient, remainder;

    quotient = value / INDEX_PER_SIZE[level];
    remainder = value % INDEX_PER_SIZE[level];

    if(!(branch->index[quotient])) {
        branch->children_size++;
        if(level == LAST_BRANCH_LEVEL) {
            branch->index[quotient] = init_leaf_node(original);
        } else {
            branch->index[quotient] = init_branch_node(level + 1, original);
        }
    }

    if (level == LAST_BRANCH_LEVEL) {
        return search_and_insert_at_leaf((leaf_node)branch->index[quotient], remainder);
    } else {
        return search_and_insert((branch_node)branch->index[quotient], level + 1, remainder, original);
    }
}

int search_and_insert_at_leaf(leaf_node leaf, unsigned int value)
{
    int exist_flag = 0;
    unsigned int target_bit;

    target_bit = 1 << value; 

    if (!(leaf->data & target_bit)) {
        leaf->data |= target_bit;
        exist_flag = 1;
    }

    return exist_flag;
}

//
// remove element from set
//
void delete_num(root_node root, unsigned int value)
{
    unsigned int quotient, remainder;

    quotient = value / INDEX_PER_SIZE[0];
    remainder = value % INDEX_PER_SIZE[0];

    if (!(root->index[quotient])) return;

    if (search_and_remove((branch_node)root->index[quotient], 1, remainder, value)) {
        root->size--;
    }
}

int search_and_remove(branch_node branch, int level, unsigned int value, unsigned int original)
{
    unsigned int quotient, remainder, result;

    quotient = value / INDEX_PER_SIZE[level];
    remainder = value % INDEX_PER_SIZE[level];

    if (!(branch->index[quotient])) return;

    if (level == LAST_BRANCH_LEVEL) {
        return search_and_remove_at_leaf((leaf_node)branch->index[quotient], remainder);
    } else {
        return search_and_remove((branch_node)branch->index[quotient], level + 1, remainder, original);
    }
}

int search_and_remove_at_leaf(leaf_node leaf, unsigned int value)
{
    int exist_flag = 0;
    unsigned int target_bit;

    target_bit = 1 << value; 

    if ((leaf->data & target_bit)) {
        leaf->data ^= target_bit;
        exist_flag = 1;
    }

    return exist_flag;
}
