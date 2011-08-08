//************************************
//         rejuicer.h
//
//       Tsukasa OISHI
//
//         2010/03/17
//************************************

// max children size at node
#define ROOT_NODE_SIZE 8
#define BRANCH_NODE_SIZE 256
// tree bottom level
#define LAST_BRANCH_LEVEL 3

// RejuicerSet class object
static VALUE rb_cRejuicerSet;

// size of a child
static unsigned int INDEX_PER_SIZE[] = {0x20000000, 0x200000, 0x2000, 0x20};

// leaf node
typedef struct _leaf_node {
    unsigned int num;   // offset
    unsigned int data;  // bit operation
} *leaf_node;

// branch node
typedef struct _branch_node {
    unsigned int num;               // offset
    unsigned int level;
    unsigned int children_size;
    void *index[BRANCH_NODE_SIZE];  // children pointer
} *branch_node;

typedef struct _root_node {
    unsigned int size;              // number of elements in set
    unsigned int children_size;     
    void *index[ROOT_NODE_SIZE];    // children pointer
} *root_node;

// initialize
void init_root_node(root_node);
void *init_branch_node(int, unsigned int);
void *init_branch_node2(int, unsigned int);
void *init_leaf_node(unsigned int);
void *init_leaf_node2(unsigned int);

// insert element into set
void add_num(root_node, unsigned int);
int search_and_insert(branch_node, int, unsigned int, unsigned int);
int search_and_insert_at_leaf(leaf_node, unsigned int);

// remove element from set
void delete_num(root_node, unsigned int);
int search_and_remove(branch_node, int, unsigned int, unsigned int);
int search_and_remove_at_leaf(leaf_node, unsigned int);

// output Array object from internal set
void to_array(root_node, VALUE);
void search_and_get_array(branch_node, VALUE);
void search_and_get_array_at_leaf(leaf_node, VALUE);

// intersection
void intersection(root_node, root_node, root_node);
void intersection_branch_node(root_node, branch_node, branch_node, branch_node);
void intersection_leaf_node(root_node, leaf_node, leaf_node, leaf_node);
unsigned int bit_count(unsigned int);

// memory free
void destroy_all(root_node);
void destroy_branch(branch_node);

//-----------------------------------------------------------
// Ruby Methods
// ----------------------------------------------------------

static VALUE t_new(int, VALUE *, VALUE);
static VALUE t_add(VALUE, VALUE);
static VALUE t_delete(VALUE, VALUE);
static VALUE t_intersection(VALUE, VALUE);
static VALUE t_to_a(VALUE);
static VALUE t_size(VALUE);
static VALUE t_empty(VALUE);
void Init_rejuicer_set(void);

