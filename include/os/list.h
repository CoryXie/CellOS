/* list.h - linked list utils */

#ifndef _LIST_H
#define _LIST_H

/** Structure containing a circular doubly linked list. */

typedef struct list
    {
    struct list *prev;        /**< Pointer to previous entry. */
    struct list *next;        /**< Pointer to next entry. */
    } list_t;

/** Iterates over a list, setting iter to the list entry on each iteration. */

#define LIST_FOREACH(list, iter)        \
    for(list_t *iter = (list)->next; iter != (list); iter = iter->next)

/** Iterates over a list in reverse, setting iter to the list entry on each
 *  iteration.
 */

#define LIST_FOREACH_REVERSE(list, iter)        \
    for(list_t *iter = (list)->prev; iter != (list); iter = iter->prev)

/** Iterates over a list, setting iter to the list entry on each iteration.
 * @note        Safe to use when the loop may modify the list.
 */

#define LIST_FOREACH_SAFE(list, iter)        \
    for(list_t *iter = (list)->next, *_##iter = iter->next; \
        iter != (list); iter = _##iter, _##iter = _##iter->next)

/** Iterates over a list in reverse, setting iter to the list entry on each
 *  iteration.
 * @note        Safe to use when the loop may modify the list.
 */

#define LIST_FOREACH_SAFE_REVERSE(list, iter)        \
    for(list_t *iter = (list)->prev, *_##iter = iter->prev; \
        iter != (list); iter = _##iter, _##iter = _##iter->prev)

/** Initialises a statically declared linked list. */

#define LIST_INITIALISER(_var)            \
    {                                   \
        .prev = &_var, \
        .next = &_var, \
    }

/** Statically declares a new linked list. */

#define LIST_DECLARE(_var)            \
    list_t _var = LIST_INITIALISER(_var)

/** Gets a pointer to the structure containing a list header, given the
 *  structure type and the member name of the list header.
 */

#define LIST_ENTRY(iter, type, member)        \
    (type *)((char *)iter - offsetof(type, member))

/** Checks whether the given list is empty. */

#define LIST_EMPTY(list)            \
    (((list)->prev == (list)) && ((list)->next) == (list))

/** Internal part of list_remove(). */

static inline void list_real_remove(list_t *entry)
    {
    entry->prev->next = entry->next;
    entry->next->prev = entry->prev;
    }

/** Initialises a linked list.
 *
 * Initialises the given linked list header structure.
 *
 * @param list        List to initialise.
 */

static inline void list_init(list_t *list)
    {
    list->prev = list->next = list;
    }

/** Add an entry to a list.
 *
 * Adds an entry to a list before the given entry.
 *
 * @param exist        Existing entry to add before.
 * @param entry        Entry to append.
 */

static inline void list_add_before(list_t *exist, list_t *entry)
    {
    list_real_remove(entry);

    exist->prev->next = entry;
    entry->next = exist;
    entry->prev = exist->prev;
    exist->prev = entry;
    }

/** Add an entry to a list.
 *
 * Adds an entry to a list after the given entry.
 *
 * @param exist        Existing entry to add after.
 * @param entry        Entry to append.
 */

static inline void list_add_after(list_t *exist, list_t *entry)
    {
    list_real_remove(entry);

    exist->next->prev = entry;
    entry->next = exist->next;
    entry->prev = exist;
    exist->next = entry;
    }

/** Append an entry to a list.
 *
 * Adds an entry to a list before the list header, thus appending it to the
 * list.
 *
 * @param list        List to append to.
 * @param entry        Entry to append.
 */

static inline void list_append(list_t *list, list_t *entry)
    {
    list_init(entry);
    list_add_before(list, entry);
    }

/** Prepend an entry to a list.
 *
 * Adds an entry to a list after the list header, thus prepending it to the
 * list.
 *
 * @param list        List to prepend to.
 * @param entry        Entry to prepend.
 */

static inline void list_prepend(list_t *list, list_t *entry)
    {
    list_add_after(list, entry);
    }

/** Remove a list entry.
 *
 * Removes a list header from its containing list.
 *
 * @param entry        Entry to remove.
 */

static inline void list_remove(list_t *entry)
    {
    list_real_remove(entry);
    list_init(entry);
    }

#endif /* __RTLD_LIST_H */

