/* rbtree.c - Red Black tree management */

#include <sys.h>
#include <os/rbtree.h>

/*
 * Operations on rbnode_t struct
 */
 
/*!
 * Construct of a red-black tree node
 *
 * \param object The object stored in the node
 */
void rbnode_init
    (
    rbnode_t * node,
    void * object
    )
    {
    node->object = object;
    node->color = RB_RED;
    node->parent = node->right = node->left = NULL;
    }

/*!
 * Construct of a red-black tree node
 *
 * \param object The object stored in the node
 * \param color The color of the node
 */
rbnode_t * rbnode_create
    (
    void * object,
    rbcolor_t color
    )
    {
    rbnode_t * node = (rbnode_t *) kmalloc(sizeof(rbnode_t));

    if (!node)
        {
        printk("Not enough memory!\n");
        return NULL;
        }

    node->object = object;
    node->color = color;
    node->parent = node->right = node->left = NULL;

    return node;
    }

/*! Recursively destroy the entire sub-tree */
void rbnode_destroy
    (
    rbnode_t * node
    )
    {
    if (!node)
        return;

    rbnode_destroy(node->right);

    rbnode_destroy(node->left);

    kfree(node);
    }

/*!
 * Calculate the depth of the sub-tree spanned by the given node
 *
 * \param node The sub-tree root
 *
 * \return The sub-tree depth
 */
int rbnode_depth
    (
    rbnode_t * node
    )
    {
    /* Recursively calculate the depth of the left and right sub-trees */

    int  iRightDepth = (node->right) ? rbnode_depth(node->right) : 0;

    int  iLeftDepth = (node->left) ? rbnode_depth(node->left) : 0;

    /* Return the maximal child depth + 1 (the current node) */
    return ((iRightDepth > iLeftDepth) ?
            (iRightDepth + 1) : (iLeftDepth + 1));
    }

/*!
 * Get the leftmost node in the sub-tree spanned by the given node
 *
 * \param node The sub-tree root
 *
 * \return The sub-tree minimum
 */
rbnode_t * rbnode_minimum
    (
    rbnode_t * node
    )
    {
    while (node->left)
        node = node->left;

    return node;
    }

/*!
 * Get the rightmost node in the sub-tree spanned by the given node
 *
 * \param node The sub-tree root
 *
 * \return The sub-tree maximum
 */
rbnode_t * rbnode_maximum
    (
    rbnode_t * node
    )
    {
    while (node->right)
        node = node->right;
    return node;
    }

/* Replace the object */
void rbnode_replace
    (
    rbnode_t * node, 
    void * object
    )
    {
    /* Make sure the replacement does not violate the tree order */

    /* Replace the object at node */

    node->object = object;
    }

/*!
 * Get the next node in the tree (according to the tree order)
 *
 * \param node The current node
 *
 * \return The successor node, or NULL if node is the tree maximum
 */
rbnode_t * rbnode_successor
    (
    rbnode_t * node
    )
    {
    rbnode_t * succ_node;

    if (node->right)
        {
        /*
         * If there is a right child, the successor is the minimal
         * object in the sub-tree spanned by this child.
         */
        succ_node = node->right;

        while (succ_node->left)
            succ_node = succ_node->left;
        }
    else
        {
        /*
         * Otherwise, go up the tree until reaching the parent
         * from the left direction.
         */
        rbnode_t * prev_node = node;

        succ_node = node->parent;

        while (succ_node && prev_node == succ_node->right)
            {
            prev_node = succ_node;
            succ_node = succ_node->parent;
            }
        }

    return (succ_node);
    }

/*!
 * Get the previous node in the tree (according to the tree order)
 *
 * \param node The current node
 *
 * \return The predecessor node, or NULL if node is the tree minimum
 */
rbnode_t * rbnode_predecessor
    (
    rbnode_t * node
    )
    {
    rbnode_t * pred_node;

    if (node->left)
        {
        /*
         * If there is a left child, the predecessor is the maximal
         * object in the sub-tree spanned by this child.
         */
        pred_node = node->left;
        while (pred_node->right)
            pred_node = pred_node->right;
        }
    else
        {
        /*
         * Otherwise, go up the tree until reaching the parent
         * from the right direction.
         */
        rbnode_t * prev_node = node;

        pred_node = node->parent;

        while (pred_node && prev_node == pred_node->left)
            {
            prev_node = pred_node;
            pred_node = pred_node->parent;
            }
        }

    return (pred_node);
    }

/*!
 * Duplicate the entire sub-tree rooted at the given node
 *
 * \param node The sub-tree root
 *
 * \return A pointer to the duplicated sub-tree root
 */
rbnode_t * rbnode_duplicate
    (
    rbnode_t * node
    )
    {
    /* Create a node of the same color, containing the same object */
    rbnode_t * dup_node = rbnode_create(node->object, node->color);

    if (!dup_node)
        return NULL;

    /* Duplicate the children recursively */
    if (node->right)
        {
        dup_node->right = rbnode_duplicate (node->right);
        dup_node->right->parent = dup_node;
        }
    else
        {
        dup_node->right = NULL;
        }

    if (node->left)
        {
        dup_node->left = rbnode_duplicate(node->left);
        dup_node->left->parent = dup_node;
        }
    else
        {
        dup_node->left = NULL;
        }

    /* Return the duplicated node */
    return dup_node;
    }

/*!
 * Traverse a red-black sub-tree
 *
 * \param node The sub-tree root
 *
 * \param op The operation to perform on each object in the sub-tree
 */
void rbnode_traverse
    (
    rbnode_t * node, 
    rb_operation op
    )
    {
    if (!node)
        return;

    rbnode_traverse(node->left, op);

    if (op) (op)(node->object);

    rbnode_traverse(node->right, op);
    }

/*
 * Operations on rbtree struct
 */

/*!
 * Initialize a red-black tree with a comparision function
 *
 * \param tree The tree
 *
 * \param comp The comparision function
 */
void rbtree_init
    (
    rbtree_t * tree,
    rb_compare comp
    )
    {
    tree->compare = comp;
    tree->size = 0;
    tree->root = NULL;
    }

/*!
 * Construct a red-black tree with a comparison object
 *
 * \param comp Comparison function to be used by the tree
 *
 * \return The newly constructed  tree
 */
rbtree_t * rbtree_create
    (
    rb_compare comp
    )
    {
    rbtree_t * tree = (rbtree_t *) kmalloc(sizeof(rbtree_t));

    if (!tree)
        {
        printk("Not enough memory!\n");

        return NULL;
        }

    rbtree_init(tree, comp);

    return tree;
    }

/*!
 * Clean a red-black tree [takes O(n) operations]
 *
 * Remove all objects from a black-red tree
 *
 * \param tree The tree
 */
void rbtree_clean
    (
    rbtree_t * tree
    )
    {
    if (tree->root)
        rbnode_destroy(tree->root);

    tree->root = NULL;
    tree->size = 0;
    }

/*!
 * Destroy a red-black tree
 *
 * \param tree The tree
 */
void rbtree_destroy
    (
    rbtree_t * tree
    )
    {
    rbtree_clean(tree);
    kfree(tree);
    }

/*!
 * Get the size of the tree [takes O(1) operations]
 *
 * \param tree The tree
 *
 * \return The number of objects stored in the tree
 */
int rbtree_size(rbtree_t * tree)
    {
    return tree->size;
    }

/*!
 * Get the depth of the tree [takes O(n) operations]
 *
 * \param tree The tree
 *
 * \return The length of the longest path from the root to a leaf node
 */
int rbtree_depth
    (
    rbtree_t * tree
    )
    {
    if (!(tree->root))
        return 0;
    return rbnode_depth(tree->root);
    }


/*!
 * Check whether the tree contains an object [takes O(log n) operations]
 *
 * \param tree The tree
 *
 * \param object The query object
 *
 * \return (true) if an equal object is found in the tree, otherwise (false)
 */
BOOL rbtree_contains
    (
    rbtree_t * tree,
    void * object
    )
    {
    return (rbtree_find(tree, object) != NULL);
    }

/*!
 * Add an node to the tree [takes O(log n) operations]
 *
 * \param tree The tree
 *
 * \param new_node The node to be inserted (initialized by rbnode_init)
 *
 * \return the inserted object node
 */
rbnode_t * rbtree_insert_node
    (
    rbtree_t * tree, 
    rbnode_t * new_node
    )
    {
    rbnode_t * cur_node;

    if (new_node->tree == tree)
        return;
    
    if (!(tree->root))
        {
        /*
         * Assign a new root node.
         * Notice that the root is always black
         */
         
        new_node->color = RB_BLACK;

        tree->root = new_node;
        tree->size = 1;
        
        new_node->tree = tree;
        
        return new_node;
        }

    /* Find a place for the new node, and insert it as a red leaf */
    cur_node = tree->root;

    while (cur_node)
        {
        /*
         * Compare inserted object with the object stored
         * in the current node
         */
        if ((tree->compare)(new_node->object, cur_node->object) > 0)
            {
            if (!(cur_node->left))
                {
                /*
                 * Insert the new leaf as the left
                 * child of the current node
                 */
                cur_node->left = new_node;
                new_node->parent = cur_node;

                /* Terminate the while loop */
                cur_node = NULL;
                }
            else
                {
                /* Go to the left sub-tree */
                cur_node = cur_node->left;
                }
            }
        else
            {
            if (!(cur_node->right))
                {
                /*
                 * Insert the new leaf as the right
                 * child of the current node
                 */
                cur_node->right = new_node;
                new_node->parent = cur_node;

                /* Terminate the while loop */
                cur_node = NULL;
                }
            else
                {
                /* Go to the right sub-tree */
                cur_node = cur_node->right;
                }
            }
        }

    /* Mark that a new node was added */
    tree->size++;

    /* Fix up the tree properties */
    rbtree_insert_fixup(tree, new_node);
    
    new_node->tree = tree;
    
    return new_node;
    }

/*!
 * Insert an object to the tree [takes O(log n) operations]
 *
 * \param tree The tree
 *
 * \param object The object to be inserted
 *
 * \return the inserted object node
 */
rbnode_t * rbtree_insert_object
    (
    rbtree_t * tree, 
    void * object
    )
    {
    rbnode_t * cur_node;
    rbnode_t * new_node;

    if (!(tree->root))
        {
        /*
         * Assign a new root node.
         * Notice that the root is always black
         */
        new_node = rbnode_create(object, RB_BLACK);

        if (!new_node)
            return NULL;

        tree->root = new_node;
        tree->size = 1;
        
        new_node->tree = tree;
        
        return new_node;
        }

    /* Find a place for the new object, and insert it as a red leaf */
    cur_node = tree->root;

    new_node = rbnode_create(object, RB_RED);
    if (!new_node)
        return NULL;

    while (cur_node)
        {
        /*
         * Compare inserted object with the object stored
         * in the current node
         */
        if ((tree->compare)(object, cur_node->object) > 0)
            {
            if (!(cur_node->left))
                {
                /*
                 * Insert the new leaf as the left
                 * child of the current node
                 */
                cur_node->left = new_node;
                new_node->parent = cur_node;

                /* Terminate the while loop */
                cur_node = NULL;
                }
            else
                {
                /* Go to the left sub-tree */
                cur_node = cur_node->left;
                }
            }
        else
            {
            if (!(cur_node->right))
                {
                /*
                 * Insert the new leaf as the right
                 * child of the current node
                 */
                cur_node->right = new_node;
                new_node->parent = cur_node;

                /* Terminate the while loop */
                cur_node = NULL;
                }
            else
                {
                /* Go to the right sub-tree */
                cur_node = cur_node->right;
                }
            }
        }

    /* Mark that a new node was added */
    tree->size++;

    /* Fix up the tree properties */
    rbtree_insert_fixup(tree, new_node);
    
    new_node->tree = tree;
    
    return new_node;
    }

/*! 
 * Insert a new object to the tree as the a successor of a given node
 *
 * \param tree The tree
 *
 * \return The new node
 */
rbnode_t * rbtree_insert_successor_object_at
    (
    rbtree_t * tree,
    rbnode_t * at_node, 
    void * object
    )
    {
    rbnode_t * parent;
    rbnode_t * new_node;

    if (!(tree->root))
        {
        /* Assign a new root node. 
         * Notice that the root is always black 
         */
        new_node = rbnode_create(object, RB_BLACK);
        
        if (!new_node) 
            return NULL;
        
        tree->root = new_node;
        tree->size = 1;

        new_node->tree = tree;

        return new_node;
        }

    /* Insert the new object as a red leaf, being the successor of node */
    new_node = rbnode_create(object, RB_RED);
    
    if (!new_node) 
        return NULL;

    if (!at_node)
        {
        /* 
         * The new node should become the tree minimum: Place is as the left
         * child of the current minimal leaf.
         */
        parent = rbnode_minimum(tree->root);
        
        parent->left = new_node;
        }
    else
        {
        /* Make sure the insertion does not violate the tree order */

        /* 
         * In case given node has no right child, place the new node as its
         * right child. Otherwise, place it at the leftmost position at the
         * sub-tree rooted at its right side.
         */
        if (!at_node->right)
            {
            parent = at_node;
            parent->right = new_node;
            }
        else
            {
            parent = rbnode_minimum(at_node->right);
            parent->left = new_node;
            }
        }

    new_node->parent = parent;

    /* Mark that a new node was added */
    tree->size++;

    /* Fix up the tree properties */
    rbtree_insert_fixup(tree, new_node);

    new_node->tree = tree;

    return new_node;
    }

/*! 
 * Insert a new object to the tree as the a predecessor of a given node
 *
 * \param tree The tree
 *
 * \return The new node
 */
rbnode_t * rbtree_insert_predecessor_object_at
    (
    rbtree_t * tree,
    rbnode_t * at_node, 
    void * object
    )
    {
    rbnode_t * parent;
    rbnode_t * new_node;

    if (!(tree->root))
        {
        /* Assign a new root node. Notice that the root is always black */
        new_node = rbnode_create(object, RB_BLACK);
        
        if (!new_node) 
            return NULL;
        
        tree->root = new_node;
        tree->size = 1;

        new_node->tree = tree;
        
        return new_node;
        }

    /* Insert the new object as a red leaf, being the predecessor of at_node */
    new_node = rbnode_create(object, RB_RED);
    
    if (!new_node) 
        return NULL;

    if (!at_node)
        {
        /* The new node should become the tree maximum: Place is as the right
         * child of the current maximal leaf.
         */
        parent = rbnode_maximum(tree->root);
        parent->right = new_node;
        }
    else
        {
        /* Make sure the insertion does not violate the tree order */

        /* In case given node has no left child, place the new node as its
         * left child. Otherwise, place it at the rightmost position at the
         * sub-tree rooted at its left side.
         */
        if (!(at_node->left))
            {
            parent = at_node;
            parent->left = new_node;
            }
        else
            {
            parent = rbnode_maximum (at_node->left);
            parent->right = new_node;
            }
        }

    new_node->parent = parent;

    /* Mark that a new node was added */
    tree->size++;

    /* Fix up the tree properties */
    rbtree_insert_fixup(tree, new_node);

    new_node->tree = tree;

    return new_node;
    }


/*! 
 * Remove an object from the tree [takes O(log n) operations]
 *
 * \param tree The tree
 *
 * \param object The object to be removed
 *
 * \pre The object should be contained in the tree
 */
void rbtree_remove
    (
    rbtree_t * tree, 
    void * object
    )
    {
    /* find the node */
    rbnode_t * node = rbtree_find(tree, object); 

    /* remove the node */

    if (node) 
        rbtree_remove_node(tree, node);
    }

/*! 
 * Remove the node stored in the given tree 
 *
 * \param tree The tree
 *
 * \param node The node to be removed from the tree
 */
void rbtree_remove_node
    (
    rbtree_t * tree, 
    rbnode_t * node
    )
    {
    rbnode_t * child = NULL;

    if (node->tree != tree)
        return;
    
    /* 
     * In case of deleting the single object stored in the tree, 
     * free the root, thus emptying the tree.
     */
    if (tree->size == 1)
        {
        if (node == tree->root)
            {
            tree->root = NULL;
            tree->size = 0;
            node->tree = NULL;
            }
        return;
        }

    /* Remove the given node from the tree */
    if (node->left && node->right)
        {
        /* 
         * If the node we want to remove has two children,
         * find its successor, which is the leftmost child 
         * in its right sub-tree and has at most one child 
         * (it may have a right child).
         */
        rbnode_t * succ_node = rbnode_minimum(node->right);

        /* 
         * Now physically swap node and its successor. Notice 
         * this may temporarily violate the tree properties, 
         * but we are going to remove node anyway. This way we 
         * have moved node to a position were it is more convinient
         * to delete it.
         */
        int immediate_succ = (node->right == succ_node);
        rbnode_t * succ_parent = succ_node->parent;
        rbnode_t * succ_left = succ_node->left;
        rbnode_t * succ_right = succ_node->right;
        rbcolor_t succ_color = succ_node->color;

        succ_node->parent = node->parent;
        succ_node->left = node->left;
        succ_node->right = immediate_succ ? node : node->right;
        succ_node->color = node->color;

        node->parent = immediate_succ ? succ_node : succ_parent;
        node->left = succ_left;
        node->right = succ_right;
        node->color = succ_color;

        if (!immediate_succ)
            {
            if (succ_node == node->parent->left)
                node->parent->left = node;
            else
                node->parent->right = node;
            }

        if (node->left)
            node->left->parent = node;
        if (node->right)
            node->right->parent = node;

        if (succ_node->parent)
            {
            if (node == succ_node->parent->left)
                succ_node->parent->left = succ_node;
            else
                succ_node->parent->right = succ_node;
            }
        else
            {
            tree->root = succ_node;
            }

        if (succ_node->left)
            succ_node->left->parent = succ_node;
        if (succ_node->right)
            succ_node->right->parent = succ_node;
        }

    /* At this stage, the node we are going to remove has at most one child */
    child = (node->left) ? node->left : node->right;

    /* 
     * Splice out the node to be removed, by linking its parent straight to the
     * removed node's single child.
     */
    if (child)
        child->parent = node->parent;

    if (!(node->parent))
        {
        /* If we are deleting the root, make the child the new tree node */
        tree->root = child;
        }
    else
        {
        /* Link the removed node parent to its child */
        if (node == node->parent->left)
            {
            node->parent->left = child;
            }
        else
            {
            node->parent->right = child;
            }
        }

    /* 
     * Fix-up the red-black properties that may have been damaged: If we have
     * just removed a black node, the black-depth property is no longer valid.
     */
    if (node->color == RB_BLACK && child)
        rbtree_remove_fixup(tree, child);

    /* 
     * Delete the un-necessary node (we nullify both its children because the
     * node's destructor is recursive).
     */
    node->left = NULL;
    node->right = NULL;
    node->tree = NULL;
    
    /* Descrease the number of objects in the tree */
    tree->size--;
    }

/*! 
 * Get a handle to the tree minimum [takes O(log n) operations]
 *
 * \param tree The tree
 *
 * \return the minimal object in the tree, or a NULL if the tree is empty
 */
rbnode_t * rbtree_minimum(rbtree_t * tree)
    {
    if (!(tree->root))
        return NULL;

    /* Return the leftmost leaf in the tree */
    return rbnode_minimum(tree->root);
    }

/*! 
 * Get a handle to the tree maximum [takes O(log n) operations]
 *
 * \param tree The tree
 *
 * \return the maximal object in the tree, or a NULL if the tree is empty
 */
rbnode_t * rbtree_maximum(rbtree_t * tree)
    {
    if (!(tree->root))
        return NULL;

    /* Return the rightmost leaf in the tree */
    return rbnode_maximum(tree->root);
    }

/*! 
 * Get the next node in the tree (according to the tree order)
 *
 * [takes O(log n) operations at worst-case, but only O(1) amortized]
 *
 * \param tree The tree
 *
 * \param node The current object
 *
 * \return The successor node, or a NULL, if we are at the tree maximum
 */
rbnode_t * rbtree_successor
    (
    rbtree_t *tree,
    rbnode_t *node
    )
    {
    rbnode_t *succ;

    /*
     * If there is a right child, the successor is the minimal
     * object in the sub-tree spanned by this child.
     */

    if (node->right != NULL)
        return rbnode_minimum(node->right);

    /*
     * Otherwise, go up the tree until reaching the parent
     * from the left direction.
     */

    succ = node->parent;

    while (succ != NULL && node == succ->right)
        {
        node = succ;
        succ = succ->parent;
        }

    return succ;
    }

/*! 
 * Get the previous node in the tree (according to the tree order)
 *
 * [takes O(log n) operations at worst-case, but only O(1) amortized]
 *
 * \param tree The tree
 *
 * \param node The current object
 *
 * \return The predecessor node, or a NULL, if we are at the tree minimum
 */
rbnode_t * rbtree_predecessor
    (
    rbtree_t * tree,
    rbnode_t * node
    )
    {
    rbnode_t * pre_node;
    rbnode_t * root = tree->root;

    /* Assignment to pre_node is intentional */
    if ((pre_node = node->left) != NULL) 
        { 
        /* Returns the maximum of the left subtree of node */
        
        while (pre_node->right != NULL) 
            { 
            pre_node = pre_node->right;
            }
          
        return(pre_node);
        } 
    else 
        {
        pre_node = node->parent;
        
        while (node == pre_node->left) 
            { 
            if (pre_node == root) 
                return (NULL); 
            
            node = pre_node;
            
            pre_node = pre_node->parent;
            }
        
        return (pre_node);
        }
    }

/*! 
 * Find a node that contains the given object
 *
 * \param tree The tree
 *
 * \param object The desired object
 *
 * \return A node that contains the given object, or NULL if no such object
 *         is found in the tree
 */
rbnode_t * rbtree_find(rbtree_t * tree, void * object)
    {
    rbnode_t * cur_node = tree->root;
    int comp_result;

    while (cur_node)
        {
        /* In case of equality, we can return the current node. */
        if ((comp_result = (tree->compare)(object, cur_node->object)) == 0)
            return cur_node;
        
        /* Go down to the left or right child. */
        cur_node = (comp_result > 0) ? 
                    cur_node->left : cur_node->right;
        }

    /* If we reached here, the object is not found in the tree */
    return NULL;
    }

/* Left-rotate the sub-tree spanned by the given node:
 * ========================================================
 *          |          RoateRight(y)            |
 *          y         -------------->           x
 *        /   \                               /   \       .
 *       x     T3       RoatateLeft(x)       T1    y      .
 *     /   \          <--------------            /   \    .
 *    T1    T2                                  T2    T3
 * ========================================================
 * \param tree The tree
 *
 * \param node The sub-tree root
 */
void rbtree_rotate_left
    (
    rbtree_t * tree, 
    rbnode_t * x_node
    )
    {
    /* Get the right child of the node */
    rbnode_t * y_node = x_node->right;

    /* Change its left subtree (T2) to x's right subtree */
    x_node->right = y_node->left;

    /* Link T2 to its new parent x */
    if (y_node->left != NULL)
        y_node->left->parent = x_node;

    /* Assign x's parent to be y's parent */
    y_node->parent = x_node->parent;

    if (!(x_node->parent))
        {
        /* Make y the new tree root */
        tree->root = y_node;
        }
    else
        {
        /* Assign a pointer to y from x's parent */
        if (x_node == x_node->parent->left)
            {
            x_node->parent->left = y_node;
            }
        else
            {
            x_node->parent->right = y_node;
            }
        }

    /* Assign x to be y's left child */
    y_node->left = x_node;
    x_node->parent = y_node;
    }

/* 
 * Right-rotate the sub-tree spanned by the given node 
 * ========================================================
 *          |          RoateRight(y)            |
 *          y         -------------->           x
 *        /   \                               /   \       .
 *       x     T3       RoatateLeft(x)       T1    y      .
 *     /   \          <--------------            /   \    .
 *    T1    T2                                  T2    T3
 * ========================================================
 * \param tree The tree
 *
 * \param node The sub-tree root
 */
void rbtree_rotate_right
    (
    rbtree_t * tree, 
    rbnode_t * y_node
    )
    {
    /* Get the left child of the node */
    rbnode_t * x_node = y_node->left;

    /* Change its right subtree (T2) to y's left subtree */
    y_node->left = x_node->right;

    /* Link T2 to its new parent y */
    if (x_node->right != NULL)
        x_node->right->parent = y_node;

    /* Assign y's parent to be x's parent */
    x_node->parent = y_node->parent;

    if (!(y_node->parent))
        {
        /* Make x the new tree root */
        tree->root = x_node;
        }
    else
        {
        /* Assign a pointer to x from y's parent */
        if (y_node == y_node->parent->left)
            {
            y_node->parent->left = x_node;
            }
        else
            {
            y_node->parent->right = x_node;
            }
        }

    /* Assign y to be x's right child */
    x_node->right = y_node;
    y_node->parent = x_node;
    }

/*!
 * Fix-up the red-black tree properties after an insertion operation
 * so it maintains the red-black properties.
 *
 * \param tree The tree
 *
 * \param node The node that has just been inserted to the tree
 *
 * \pre The color of node must be red
 */
void rbtree_insert_fixup
    (
    rbtree_t * tree,
    rbnode_t * node
    )
    {
    /*
     * Fix the red-black propreties: we may have inserted a red
     * leaf as the child of a red parent - so we have to fix the
     * coloring of the parent recursively.
     */
    rbnode_t * curr_node = node;
    rbnode_t * grandparent;
    rbnode_t *uncle;

    ASSERT(node && node->color == RB_RED);

    while (curr_node != tree->root &&
            curr_node->parent->color == RB_RED)
        {
        /*
         * Get a pointer to the current node's grandparent
         * (notice the root is always black, so the red
         * parent must have a parent).
         */
        grandparent = curr_node->parent->parent;

        if (curr_node->parent == grandparent->left)
            {
            /* If the red parent is a left child, the uncle is the right child of
            * the grandparent.
            */
            uncle = grandparent->right;

            if (uncle && uncle->color == RB_RED)
                {
                /* If both parent and uncle are red, color them black and color the
                 * grandparent red.
                 * In case of a NULL uncle, we treat it as a black node.
                 */
                curr_node->parent->color = RB_BLACK;
                uncle->color = RB_BLACK;
                grandparent->color = RB_RED;

                /* Move to the grandparent */
                curr_node = grandparent;
                }
            else
                {
                /* Make sure the current node is a right child. If not, left-rotate
                 * the parent's sub-tree so the parent becomes the right child of the
                 * current node (see _rotate_left).
                 */
                if (curr_node == curr_node->parent->right)
                    {
                    curr_node = curr_node->parent;
                    rbtree_rotate_left(tree, curr_node);
                    }

                /* Color the parent black and the grandparent red */
                curr_node->parent->color = RB_BLACK;
                grandparent->color = RB_RED;

                /* Right-rotate the grandparent's sub-tree */
                rbtree_rotate_right(tree, grandparent);
                }
            }
        else
            {
            /* If the red parent is a right child, the uncle is the left child of
            * the grandparent.
            */
            uncle = grandparent->left;

            if (uncle && uncle->color == RB_RED)
                {
                /* If both parent and uncle are red, color them black and color the
                 * grandparent red.
                 * In case of a NULL uncle, we treat it as a black node.
                 */
                curr_node->parent->color = RB_BLACK;
                uncle->color = RB_BLACK;
                grandparent->color = RB_RED;

                /* Move to the grandparent */
                curr_node = grandparent;
                }
            else
                {
                /* Make sure the current node is a left child. If not, right-rotate
                 * the parent's sub-tree so the parent becomes the left child of the
                 * current node.
                 */
                if (curr_node == curr_node->parent->left)
                    {
                    curr_node = curr_node->parent;
                    rbtree_rotate_right(tree, curr_node);
                    }

                /* Color the parent black and the grandparent red */
                curr_node->parent->color = RB_BLACK;
                grandparent->color = RB_RED;

                /* Left-rotate the grandparent's sub-tree */
                rbtree_rotate_left(tree, grandparent);
                }
            }
        }

    /* Make sure that the root is black */
    tree->root->color = RB_BLACK;
    }

/*! 
 * Fix-up the red-black tree properties after a removal operation
 *
 * \param tree The tree
 *
 * \param node The child of the node that has just been removed from the tree
 */
void rbtree_remove_fixup(rbtree_t * tree, rbnode_t * node)
    {
    rbnode_t * curr_node = node;
    rbnode_t * sibling;

    while (curr_node != tree->root && curr_node->color == RB_BLACK)
        {
        /* Get a pointer to the current node's sibling (notice that the node's
         * parent must exist, since the node is not the root).
         */
        if (curr_node == curr_node->parent->left)
            {
            /* If the current node is a left child, its sibling is the right
             * child of the parent.
             */
            sibling = curr_node->parent->right;

            /* Check the sibling's color. Notice that NULL nodes are treated
             * as if they are colored black.
             */
            if (sibling && sibling->color == RB_RED)
                {
                /* In case the sibling is red, color it black and rotate.
                 * Then color the parent red (and the grandparent is now black).
                 */
                sibling->color = RB_BLACK;
                curr_node->parent->color = RB_RED;
                rbtree_rotate_left(tree, curr_node->parent);
                sibling = curr_node->parent->right;
                }

            if (sibling &&
                    (!(sibling->left) || sibling->left->color == RB_BLACK) &&
                    (!(sibling->right) || sibling->right->color == RB_BLACK))
                {
                /* If the sibling has two black children, color it red */
                sibling->color = RB_RED;
                if (curr_node->parent->color == RB_RED)
                    {
                    /* 
                     * If the parent is red, we can safely color it black
                     * and terminate the fix-up process.
                     */
                    curr_node->parent->color = RB_BLACK;
                    
                    /* In order to stop the while loop */
                    curr_node = tree->root;      
                    }
                else
                    {
                    /*
                     * The black depth of the entire sub-tree rooted at the parent is
                     * now too small - fix it up recursively.
                     */
                    curr_node = curr_node->parent;
                    }
                }
            else
                {
                if (!sibling)
                    {
                    /* Take special care of the case of a NULL sibling */
                    if (curr_node->parent->color == RB_RED)
                        {
                        curr_node->parent->color = RB_BLACK;
                        
                        /* In order to stop the while loop */
                        curr_node = tree->root;    
                        }
                    else
                        {
                        curr_node = curr_node->parent;
                        }
                    }
                else
                    {
                    /* In this case, at least one of the sibling's children is red.
                     * It is therfore obvious that the sibling itself is black.
                     */
                    if (sibling->right && sibling->right->color == RB_RED)
                        {
                        /* 
                         * If the right child of the sibling is red, color it 
                         * black and rotate around the current parent.
                         */
                        sibling->right->color = RB_BLACK;
                        rbtree_rotate_left(tree, curr_node->parent);
                        }
                    else
                        {
                        /* If the left child of the sibling is red, rotate
                         * around the sibling, then rotate around the new
                         * sibling of our current node.
                         */
                        rbtree_rotate_right(tree, sibling);
                        sibling = curr_node->parent->right;
                        rbtree_rotate_left(tree, sibling);
                        }

                    /* 
                     * It is now safe to color the parent black and 
                     * to terminate the fix-up process.
                     */
                    if (curr_node->parent->parent)
                        curr_node->parent->parent->color = 
                            curr_node->parent->color;

                    curr_node->parent->color = RB_BLACK;
                    
                    /* In order to stop the while loop */
                    curr_node = tree->root;     
                    }
                }
            }
        else
            {
            /* 
             * If the current node is a right child, its sibling is the left
             * child of the parent.
             */
            sibling = curr_node->parent->left;

            /* 
             * Check the sibling's color. Notice that NULL nodes are treated
             * as if they are colored black.
             */
            if (sibling && sibling->color == RB_RED)
                {
                /* 
                 * In case the sibling is red, color it black and rotate.
                 * Then color the parent red (and the grandparent is now black).
                 */
                sibling->color = RB_BLACK;
                curr_node->parent->color = RB_RED;
                
                rbtree_rotate_right(tree, curr_node->parent);

                sibling = curr_node->parent->left;
                }

            if (sibling &&
                (!(sibling->left) || sibling->left->color == RB_BLACK) &&
                (!(sibling->right) || sibling->right->color == RB_BLACK))
                {
                /* If the sibling has two black children, color it red */
                sibling->color = RB_RED;
                if (curr_node->parent->color == RB_RED)
                    {
                    /* 
                     * If the parent is red, we can safely color it black 
                     * and terminate the fix-up process.
                     */
                    curr_node->parent->color = RB_BLACK;
                    
                    /* In order to stop the while loop */
                    curr_node = tree->root;      
                    }
                else
                    {
                    /* 
                     * The black depth of the entire sub-tree rooted at 
                     * the parent is now too small - fix it up recursively.
                     */
                    curr_node = curr_node->parent;
                    }
                }
            else
                {
                if (!sibling)
                    {
                    /* Take special care of the case of a NULL sibling */
                    if (curr_node->parent->color == RB_RED)
                        {
                        curr_node->parent->color = RB_BLACK;
                        
                        /* In order to stop the while loop */
                        curr_node = tree->root;    
                        }
                    else
                        {
                        curr_node = curr_node->parent;
                        }
                    }
                else
                    {
                    /* In this case, at least one of the sibling's children is red.
                     * It is therfore obvious that the sibling itself is black.
                     */
                    if (sibling->left && sibling->left->color == RB_RED)
                        {
                        /* If the left child of the sibling is red, color it 
                         * black and rotate around the current parent
                         */
                        sibling->left->color = RB_BLACK;
                        rbtree_rotate_right(tree, curr_node->parent);
                        }
                    else
                        {
                        /* 
                         * If the right child of the sibling is red, rotate 
                         * around the sibling, then rotate around the new 
                         * sibling of our current node.
                         */
                        rbtree_rotate_left(tree, sibling);
                        sibling = curr_node->parent->left;
                        rbtree_rotate_right(tree, sibling);
                        }

                    /* 
                     * It is now safe to color the parent black and to 
                     * terminate the fix-up process.
                     */
                    if (curr_node->parent->parent)
                        curr_node->parent->parent->color = 
                                        curr_node->parent->color;
                    
                    curr_node->parent->color = RB_BLACK;
                    
                    /* In order to stop the while loop */
                    curr_node = tree->root;       
                    }
                }
            }
        }

    /* The root can always be colored black */
    curr_node->color = RB_BLACK;
    }

/*! 
 * Traverse a red-black tree
 *
 * \param tree The tree
 *
 * \param op The operation to perform on every object of the tree (according to
 *        the tree order)
 */
void rbtree_traverse
    (
    rbtree_t * tree, 
    rb_operation op
    )
    {
    rbnode_traverse(tree->root, op);
    }

