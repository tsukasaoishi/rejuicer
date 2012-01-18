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

//-----------------------------------------------------------
// Ruby Methods
// ----------------------------------------------------------

/**
 * allocate
**/
static VALUE t_allocate(VALUE klass)
{
    VALUE obj;
    root_node root;

    obj = Data_Make_Struct(klass, struct _root_node, NULL, destroy_all, root);
    init_root_node(root);

    return obj;
}

/**
 * initialize
**/
static VALUE t_initialize(int argc, VALUE *argv, VALUE self)
{
    VALUE num;
    root_node root;

    Data_Get_Struct(self, struct _root_node, root);
    if (argc == 1) {
        while((num = rb_ary_shift(argv[0])) != Qnil) {
            add_num(root, NUM2UINT(num));
        }
    }

    return self;
}

/**
 * initialize_copy
**/
static VALUE t_initialize_copy(VALUE self, VALUE orig)
{
    root_node root, orig_set;

    Data_Get_Struct(self, struct _root_node, root);
    Data_Get_Struct(orig, struct _root_node, orig_set);
    copy_root_node(root, orig_set);

    return self;
}

/**
 * add
**/
static VALUE t_add(VALUE self, VALUE value)
{
    root_node root;
    unsigned int num;

    Data_Get_Struct(self, struct _root_node, root);
    add_num(root, NUM2UINT(value));

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
    delete_num(root, NUM2UINT(value));

    return self;
}

/**
 * intersection
**/
static VALUE t_intersection(VALUE self, VALUE other)
{
    root_node set0, set1, ret_set;
    VALUE ret;

    ret = Data_Make_Struct(rb_cRejuicerSet, struct _root_node, NULL, destroy_all, ret_set);
    init_root_node(ret_set);

    Data_Get_Struct(self, struct _root_node, set0);
    Data_Get_Struct(other, struct _root_node, set1);

    intersection(ret_set, set0, set1);

    return ret;
}

/**
 * union
**/
static VALUE t_union(VALUE self, VALUE other)
{
    root_node set0, set1, ret_set;
    VALUE ret;

    ret = Data_Make_Struct(rb_cRejuicerSet, struct _root_node, NULL, destroy_all, ret_set);
    init_root_node(ret_set);

    Data_Get_Struct(self, struct _root_node, set0);
    Data_Get_Struct(other, struct _root_node, set1);

    join(ret_set, set0, set1);

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
 * cler
**/
static VALUE t_clear(VALUE self)
{
    root_node root;

    Data_Get_Struct(self, struct _root_node, root);

    if (root->size) {
        destroy_all_branches(root);
    }

    return self;
}


/**
 * define class
**/
void Init_rejuicer_set(void) {
    rb_cRejuicerSet = rb_define_class("RejuicerSet", rb_cObject);
    rb_define_alloc_func(rb_cRejuicerSet, t_allocate);
    rb_define_private_method(rb_cRejuicerSet, "initialize", t_initialize, -1);
    rb_define_method(rb_cRejuicerSet, "initialize_copy", t_initialize_copy, 1);
    rb_define_method(rb_cRejuicerSet, "add", t_add, 1);
    rb_define_method(rb_cRejuicerSet, "delete", t_delete, 1);
    rb_define_method(rb_cRejuicerSet, "intersection", t_intersection, 1);
    rb_define_method(rb_cRejuicerSet, "union", t_union, 1);
    rb_define_method(rb_cRejuicerSet, "to_a", t_to_a, 0);
    rb_define_method(rb_cRejuicerSet, "size", t_size, 0);
    rb_define_method(rb_cRejuicerSet, "empty?", t_empty, 0);
    rb_define_method(rb_cRejuicerSet, "clear", t_clear, 0);
    rb_define_alias(rb_cRejuicerSet, "<<", "add");
    rb_define_alias(rb_cRejuicerSet, "&", "intersection");
    rb_define_alias(rb_cRejuicerSet, "|", "union");
    rb_define_alias(rb_cRejuicerSet, "length", "size");
}
