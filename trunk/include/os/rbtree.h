#ifndef _OS_RBTREE_H
#define _OS_RBTREE_H

/*
 * A red-black tree is a binary tree that satisfies the following properties:
 *
 * 1. Each node has a color, which is either red or black.
 * 2. A red node cannot have a red parent.
 * 3. The number of black nodes for every path from the tree root to a leaf
 *    is the same for all tree leaves (it is called the 'black depth' of the 
 *    tree).
 *
 * Due to propeties 2-3, the depth of a red-black tree containing n nodes
 * is bounded by 2*log_2(n).
 *
 * The rbtree requires two parmeters:
 *
 * - The contained <object> represents the object stored in the tree node.
 * - The <compare> is a function used to comapre two objects passed in
 *   as so that it can define the order of <object>s.
 */

/* Color enumeration for nodes of red-black tree */
typedef enum rbcolor 
    {
    RB_RED, 
    RB_BLACK
    } rbcolor_t;

typedef int(rb_compare)(void *, void *);
typedef void(rb_operation)(void *);

/* Representation of a node in a red-black tree */
typedef struct rbnode 
    {
    void * object;                /* The stored object */
    rbcolor_t color;              /* The color of the node */
    struct rbnode * parent;       /* Points to the parent node */
    struct rbnode * right;        /* Points to the right child */
    struct rbnode * left;         /* Points to the left child */
    } rbnode_t;

/*! Representation of a red-black tree */
typedef struct rbtree 
    {
    rbnode_t * root;                /* Pointer to the tree root */
    int size;                      /* Number of objects stored */
    rb_compare * compare;              /* Compare function */
    } rbtree_t;
  
extern rbnode_t * rbnode_create(void * object, rbcolor_t color);
extern void rbnode_destroy(rbnode_t * node);
extern int rbnode_depth(rbnode_t * node);
extern rbnode_t * rbnode_minimum(rbnode_t * node);
extern rbnode_t * rbnode_maximum(rbnode_t * node);
extern void rbnode_replace(rbnode_t * node, void * object);
extern rbnode_t * rbnode_successor(rbnode_t * node);
extern rbnode_t * rbnode_predecessor(rbnode_t * node);
extern rbnode_t * rbnode_duplicate(rbnode_t * node);
extern void rbnode_traverse(rbnode_t * node, rb_operation * op);

void rbtree_init(rbtree_t * tree, rb_compare * comp);
rbtree_t * rbtree_create(rb_compare * comp);
extern void rbtree_clean(rbtree_t * tree);
extern void rbtree_destroy(rbtree_t * tree);
extern int rbtree_size(rbtree_t * tree);
extern int rbtree_depth(rbtree_t * tree);
extern BOOL rbtree_contains(rbtree_t * tree, void * object);
extern rbnode_t * rbtree_insert(rbtree_t * tree, void * object);
extern rbnode_t * insert_successor_at(rbtree_t * tree,
                                            rbnode_t * at_node,
                                            void * object);
extern rbnode_t * insert_predecessor_at(rbtree_t * tree,
                                              rbnode_t * at_node,
                                              void * object);
extern void rbtree_remove(rbtree_t * tree, void * object);
extern rbnode_t * rbtree_minimum(rbtree_t * tree);
extern rbnode_t * rbtree_maximum(rbtree_t * tree);
extern rbnode_t * rbtree_successor(rbtree_t * tree,
                                         rbnode_t * node);
extern rbnode_t * rbtree_predecessor(rbtree_t * tree,
                                           rbnode_t * node);
extern rbnode_t * rbtree_find(rbtree_t * tree, void * object);

extern void rbtree_remove_at(rbtree_t * tree, rbnode_t * node);
extern void rbtree_rotate_left(rbtree_t * tree, rbnode_t * node);
extern void rbtree_rotate_right(rbtree_t * tree, rbnode_t * node);

extern void rbtree_insert_fixup(rbtree_t * tree, rbnode_t * node);
extern void rbtree_remove_fixup(rbtree_t * tree, rbnode_t * node);
extern void rbtree_traverse(rbtree_t * tree, rb_operation * op);

#endif /* _OS_RBTREE_H */

