#ifndef _OS_RADIXTREE_H
#define _OS_RADIXTREE_H

/*
 * Radix tree node definition.
 */
#define RADIX_TREE_MAP_SHIFT  6
#define RADIX_TREE_MAP_SIZE  (1UL << RADIX_TREE_MAP_SHIFT)
#define RADIX_TREE_MAP_MASK  (RADIX_TREE_MAP_SIZE-1)

#define RADIX_TREE_INDEX_BITS  (8 * sizeof(unsigned long))
#define RADIX_TREE_MAX_PATH (RADIX_TREE_INDEX_BITS/RADIX_TREE_MAP_SHIFT + 2)

typedef struct radixtree_node
    {
    unsigned int    count;
    void *  slots[RADIX_TREE_MAP_SIZE];
    }radixtree_node_t;

typedef struct radixtree_path 
    {
    radixtree_node_t *node;
    radixtree_node_t **slot;
    }radixtree_path_t;

typedef struct radixtree_root 
    {
    unsigned int    height;
    radixtree_node_t   *rnode;
    }radixtree_root_t;

#define RADIX_TREE_INIT()	{0, NULL}

#define RADIX_TREE(name) \
  struct radixtree_root name = RADIX_TREE_INIT()

#define INIT_RADIX_TREE(root)		\
do  {					            \
	(root)->height = 0;		        \
	(root)->rnode = NULL;		    \
    } while (0)

extern int radixtree_insert(struct radixtree_root *, unsigned long, void *);
extern void *radixtree_lookup(struct radixtree_root *, unsigned long);
extern void *radixtree_delete(struct radixtree_root *, unsigned long);
extern unsigned int radixtree_gang_lookup(struct radixtree_root *root, 
					   void **results, 
					   unsigned long first_index,
					   unsigned int max_items);
extern void radixtree_init(void);

#endif /* _OS_RADIXTREE_H */
