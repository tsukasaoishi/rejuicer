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
                ret_set->index[i] = init_branch_node2(((branch_node)base->index[i])->level, ((branch_node)base->index[i])->num);
                intersection_branch_node(ret_set, (branch_node)ret_set->index[i], (branch_node)base->index[i], (branch_node)other->index[i]);
            }
        }
    }
}

void intersection_branch_node(root_node root, branch_node ret_set, branch_node base, branch_node other)
{
    int i, count;

    for (i = 0, count = 0; i < BRANCH_NODE_SIZE || count < base->children_size; i++) {
        if (base->index[i]) {
            count++;
            if (other->index[i]) {
                if (base->level == LAST_BRANCH_LEVEL) {
                    ret_set->index[i] = init_leaf_node2(((leaf_node)base->index[i])->num);
                    intersection_leaf_node(root, (leaf_node)ret_set->index[i], (leaf_node)base->index[i], (leaf_node)other->index[i]);
                } else {
                    ret_set->index[i] = init_branch_node2(((branch_node)base->index[i])->level, ((branch_node)base->index[i])->num);
                    intersection_branch_node(root, (branch_node)ret_set->index[i], (branch_node)base->index[i], (branch_node)other->index[i]);
                }
            }
        }
    }
}

void intersection_leaf_node(root_node root, leaf_node ret_set, leaf_node base, leaf_node other)
{
    ret_set->data = base->data & other->data;
    root->size += bit_count(ret_set->data);
}

unsigned int bit_count(unsigned int x)
{
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0f0f0f0f;
    x = x + (x >> 8);
    x = x + (x >> 16);
    return x & 0x0000003F;
}

//
// memory free
//
void destroy_all(root_node root)
{
    int i, count;
    
    for(i = 0, count = 0; i < ROOT_NODE_SIZE || count < root->children_size; i++) {
        if (root->index[i]) {
            destroy_branch((branch_node)root->index[i]);
            count++;
        }
    }
    free(root);
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

//-----------------------------------------------------------
// Ruby Methods
// ----------------------------------------------------------

/**
 * new
**/
static VALUE t_new(int argc, VALUE *argv, VALUE klass)
{
    VALUE num;
    VALUE obj;
    root_node root;

    obj = Data_Make_Struct(klass, struct _root_node, NULL, destroy_all, root);
    init_root_node(root);

    if (argc == 1) {
        while((num = rb_ary_shift(argv[0])) != Qnil) {
            add_num(root, NUM2UINT(num));
        }
    }

    return obj;
}

/**
 * add
**/
static VALUE t_add(VALUE self, VALUE value)
{
    root_node root;
    unsigned int num;

    Data_Get_Struct(self, struct _root_node, root);
    if ((num = NUM2UINT(value)) != Qnil) {
        add_num(root, num);
    }

    return self;
}

/**
 * delete
 **/
static VALUE t_delete(VALUE self, VALUE value)
{
    root_node root;
    unsigned int num;

    Data_Get_Struct(self, struct _root_node, root);
    if ((num = NUM2UINT(value)) != Qnil) {
        delete_num(root, num);
    }

    return self;
}

/**
 * intersection
**/
static VALUE t_intersection(VALUE self, VALUE array)
{
    root_node set0, set1, ret_set;
    VALUE ret;

    ret = Data_Make_Struct(rb_cRejuicerSet, struct _root_node, NULL, destroy_all, ret_set);
    init_root_node(ret_set);

    Data_Get_Struct(self, struct _root_node, set0);
    Data_Get_Struct(array, struct _root_node, set1);

    intersection(ret_set, set0, set1);

    return ret;
}

/**
 * to_a
**/ 
static VALUE t_to_a(VALUE self)
{
    int i;
    root_node root;
    VALUE array;

    Data_Get_Struct(self, struct _root_node, root);
    array = rb_ary_new2(root->size);

    to_array(root, array);

    return array;
}

/**
 * size
**/
static VALUE t_size(VALUE self)
{
    root_node root;

    Data_Get_Struct(self, struct _root_node, root);

    return INT2NUM(root->size);
}

/**
 * empty?
**/
static VALUE t_empty(VALUE self)
{
    root_node root;

    Data_Get_Struct(self, struct _root_node, root);

    if (root->size == 0) {
        return Qtrue;
    } else {
        return Qfalse;
    }
}

/**
 * define class
**/
void Init_rejuicer_set(void) {
    rb_cRejuicerSet = rb_define_class("RejuicerSet", rb_cObject);
    rb_define_singleton_method(rb_cRejuicerSet, "new", t_new, -1);
    rb_define_method(rb_cRejuicerSet, "add", t_add, 1);
    rb_define_method(rb_cRejuicerSet, "delete", t_delete, 1);
    rb_define_method(rb_cRejuicerSet, "intersection", t_intersection, 1);
    rb_define_method(rb_cRejuicerSet, "to_a", t_to_a, 0);
    rb_define_method(rb_cRejuicerSet, "size", t_size, 0);
    rb_define_method(rb_cRejuicerSet, "empty?", t_empty, 0);
    rb_define_alias(rb_cRejuicerSet, "<<", "add");
    rb_define_alias(rb_cRejuicerSet, "&", "intersection");
    rb_define_alias(rb_cRejuicerSet, "length", "size");
}
