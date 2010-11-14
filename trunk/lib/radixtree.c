#include <sys.h>
#include <os/radixtree.h>

static unsigned long height_to_max_index[RADIX_TREE_MAX_PATH];

/*!
 * Allocate a radix tree node
 */
static inline radixtree_node_t * radixtree_node_alloc
    (
    struct radixtree_root *root
    )
    {
    radixtree_node_t *ret;

    ret = (radixtree_node_t *) kmalloc (sizeof (radixtree_node_t));

    if (ret) memset(ret, 0, sizeof(radixtree_node_t));

    return ret;
    }

/*!
 * Free a radix tree node
 */
static inline void radixtree_node_free
    (
    radixtree_node_t *node
    )
    {
    kfree (node);
    }


/*!
 *	Get maximum key that can be stored into a radix tree with height <height>.
 */
static inline unsigned long radixtree_max_index
    (
    unsigned int height
    )
    {
    return height_to_max_index[height];
    }

/*!
 *	Extend a radix tree so it can store key @index.
 */
static int radixtree_extend
    (
    struct radixtree_root *root,
    unsigned long index
    )
    {
    radixtree_node_t *node;
    unsigned int height;

    /* Figure out what the height should be. */

    height = root->height + 1;

    while (index > radixtree_max_index(height))
        height++;

    if (root->rnode)
        {
        do
            {
            if (!(node = radixtree_node_alloc(root)))
                return ENOMEM;

            /* Increase the height. */
            node->slots[0] = root->rnode;
            node->count = 1;
            root->rnode = node;
            root->height++;
            }
        while (height > root->height);

        }
    else
        root->height = height;

    return OK;
    }

/*!
 * Insert into a radix tree
 *
 * Insert an item into the radix tree at position <index>.
 *
 * \param root  radix tree root
 *
 * \param index index key
 *
 * \param item  item to insert
 */
int radixtree_insert
    (
    struct radixtree_root *root,
    unsigned long index,
    void *item
    )
    {
    radixtree_node_t *node = NULL;
    radixtree_node_t *tmp;
    radixtree_node_t **slot;
    unsigned int height;
    unsigned int shift;
    int error;

    /* Make sure the tree is high enough. */
    if (index > radixtree_max_index(root->height))
        {
        error = radixtree_extend(root, index);
        if (error)
            return error;
        }

    slot = &root->rnode;
    height = root->height;
    shift = (height - 1) * RADIX_TREE_MAP_SHIFT;

    while (height > 0)
        {
        if (*slot == NULL)
            {
            /* Have to add a child node. */
            if (!(tmp = radixtree_node_alloc(root)))
                return ENOMEM;

            *slot = tmp;

            if (node)
                node->count++;
            }

        /* Go a level down.  */
        node = *slot;
        slot = (radixtree_node_t **)
               (node->slots + ((index >> shift) & RADIX_TREE_MAP_MASK));
        shift -= RADIX_TREE_MAP_SHIFT;
        height--;
        }

    if (*slot != NULL)
        return EEXIST;

    if (node)
        node->count++;

    *slot = item;
    return OK;
    }

/*!
 *	Perform lookup operation on a radix tree
 *
 *	Lookup them item at the position <index> in the radix tree <root>.
 *
 *  \param root radix tree root
 *
 *	\param index index key
 */
void *radixtree_lookup
    (
    struct radixtree_root *root,
    unsigned long index
    )
    {
    unsigned int height;
    unsigned int shift;
    radixtree_node_t **slot;

    height = root->height;
    if (index > radixtree_max_index(height))
        return NULL;

    shift = (height - 1) * RADIX_TREE_MAP_SHIFT;
    slot = &root->rnode;

    while (height > 0)
        {
        if (*slot == NULL)
            return NULL;

        slot = (radixtree_node_t **)
               ((*slot)->slots + ((index >> shift) & RADIX_TREE_MAP_MASK));

        shift -= RADIX_TREE_MAP_SHIFT;

        height--;
        }

    return (void *) *slot;
    }

static unsigned int radixtree_lookup_internal
    (
    struct radixtree_root *root, 
    void **results, 
    unsigned long index,
    unsigned int max_items, 
    unsigned long *next_index
    )
    {
    unsigned int nr_found = 0;
    unsigned int shift;
    unsigned int height = root->height;
    radixtree_node_t *slot;

    shift = (height - 1) * RADIX_TREE_MAP_SHIFT;
    slot = root->rnode;

    while (height > 0)
        {
        unsigned long i = (index >> shift) & RADIX_TREE_MAP_MASK;

        for (; i < RADIX_TREE_MAP_SIZE; i++)
            {
            if (slot->slots[i] != NULL)
                break;
            
            index &= ~((1 << shift) - 1);
            index += 1 << shift;
            
            if (index == 0)
                goto out;	/* 32-bit wraparound */
            }
        
        if (i == RADIX_TREE_MAP_SIZE)
            goto out;
        
        height--;
        
        /* Bottom level: grab some items */
        if (height == 0)  	
            {
            unsigned long j = index & RADIX_TREE_MAP_MASK;

            for (; j < RADIX_TREE_MAP_SIZE; j++)
                {
                index++;
                if (slot->slots[j])
                    {
                    results[nr_found++] = slot->slots[j];
                    if (nr_found == max_items)
                        goto out;
                    }
                }
            }
        shift -= RADIX_TREE_MAP_SHIFT;
        slot = slot->slots[i];
        }
out:
    *next_index = index;
    return nr_found;
    }

/*!
 *	perform multiple lookup on a radix tree
 *
 *	Performs an index-ascending scan of the tree for present items.  Places
 *	them at *<results> and returns the number of items which were placed at
 *	*<results>.
 *
 * \param root          radix tree root
 *
 * \param results       where the results of the lookup are placed
 *
 * \param first_index   start the lookup from this key
 *
 * \param max_items     place up to this many items at *results
 */
unsigned int radixtree_gang_lookup
    (
    struct radixtree_root *root,
    void **results,
    unsigned long first_index,
    unsigned int max_items
    )
    {
    const unsigned long max_index = radixtree_max_index(root->height);
    unsigned long cur_index = first_index;
    unsigned int ret = 0;

    if (root->rnode == NULL)
        goto out;

    /* Handle special case */
    if (max_index == 0)
        {
        if (first_index == 0)
            {
            if (max_items > 0)
                {
                *results = root->rnode;
                ret = 1;
                }
            }
        goto out;
        }

    while (ret < max_items)
        {
        unsigned int nr_found;
        unsigned long next_index;	

        if (cur_index > max_index)
            break;
        
        nr_found = radixtree_lookup_internal(root, results + ret, cur_index,
                            max_items - ret, &next_index);
        ret += nr_found;
        
        if (next_index == 0)
            break;
        
        cur_index = next_index;
        }
out:
    return ret;
    }

/*!
 * Delete an item from a radix tree
 *
 * Remove the item at <index> from the radix tree rooted at <root>.
 *
 * \param root radix tree root
 *
 * \param index index key
 *
 * \return the address of the deleted item, or NULL if it was not present.
 */
void *radixtree_delete
    (
    struct radixtree_root *root,
    unsigned long index
    )
    {
    radixtree_path_t path[RADIX_TREE_MAX_PATH];
    radixtree_path_t *pathp = path;
    unsigned int height;
    unsigned int shift;
    void *ret = NULL;

    height = root->height;

    if (index > radixtree_max_index(height))
        return NULL;

    shift = (height - 1) * RADIX_TREE_MAP_SHIFT;
    pathp->node = NULL;
    pathp->slot = &root->rnode;

    while (height > 0)
        {
        if (*pathp->slot == NULL)
            return NULL;

        pathp[1].node = *pathp[0].slot;
        pathp[1].slot = (radixtree_node_t **)
                        (pathp[1].node->slots + ((index >> shift) & RADIX_TREE_MAP_MASK));
        pathp++;
        shift -= RADIX_TREE_MAP_SHIFT;
        height--;
        }

    ret = *pathp[0].slot;

    if (ret == NULL)
        return NULL;

    *pathp[0].slot = NULL;

    while (pathp[0].node && --pathp[0].node->count == 0)
        {
        pathp--;

        *pathp[0].slot = NULL;

        radixtree_node_free(pathp[1].node);
        }

    /* Empty tree, we can reset the height */

    if (root->rnode == NULL)
        root->height = 0;

    return ret;
    }

static unsigned long radixtree_max_index_internal(unsigned int height)
    {
    unsigned int tmp = height * RADIX_TREE_MAP_SHIFT;
    unsigned long index = (~0UL >> (RADIX_TREE_INDEX_BITS - tmp - 1)) >> 1;

    if (tmp >= RADIX_TREE_INDEX_BITS)
        index = ~0UL;

    return index;
    }

void radixtree_init(void)
    {
    unsigned int i;

    for (i = 0; i < NELEMENTS(height_to_max_index); i++)
        height_to_max_index[i] = radixtree_max_index_internal(i);
    }

