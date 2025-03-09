#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head) {
        return NULL;
    }
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    /* if head is NULL, do nothing */
    if (!head || list_empty(head)) {
        free(head);
        return;
    }

    /* free element_t nodes and string inside iteratively */
    element_t *entry = NULL, *safe = NULL;
    list_for_each_entry_safe (entry, safe, head, list) {
        free(entry->value);
        free(entry);
    }

    /* connect head->prev and head->next then free head */
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *element = malloc(sizeof(element_t));
    if (!element)
        return false;
    element->value = malloc(strlen(s) + 1);
    if (!element->value) {
        free(element);
        return false;
    }
    strncpy(element->value, s, strlen(s));
    element->value[strlen(s)] = '\0';

    struct list_head *next = head->next;
    next->prev = &(element->list);
    element->list.next = next;
    element->list.prev = head;
    head->next = &(element->list);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *element = malloc(sizeof(element_t));
    if (!element)
        return false;
    element->value = malloc(strlen(s) + 1);
    if (!element->value) {
        free(element);
        return false;
    }
    strncpy(element->value, s, strlen(s));
    element->value[strlen(s)] = '\0';

    struct list_head *tail = head->prev;
    tail->next = &(element->list);
    head->prev = &(element->list);
    element->list.next = head;
    element->list.prev = tail;

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }

    element_t *element = list_first_entry(head, element_t, list);
    if (sp) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    struct list_head *next = element->list.next;
    head->next = next;
    next->prev = head;
    INIT_LIST_HEAD(&(element->list));

    return element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }

    element_t *element = list_last_entry(head, element_t, list);
    if (sp) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    struct list_head *prev = element->list.prev;
    head->prev = prev;
    prev->next = head;
    INIT_LIST_HEAD(&(element->list));

    return element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head)) {
        return false;
    }

    /* find mid */
    struct list_head *fast, *slow;
    fast = slow = head;
    do {
        slow = slow->next;
        fast = fast->next->next;
    } while (fast != head && fast->next != head);

    /* connect list */
    struct list_head *next = slow->next, *prev = slow->prev;
    next->prev = prev;
    prev->next = next;

    /* free entry */
    element_t *element = list_entry(slow, element_t, list);
    if (!element)
        return false;
    if (element->value)
        free(element->value);
    free(element);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head)) {
        return false;
    }

    if (list_is_singular(head)) {
        return true;
    }

    element_t *entry, *safe, *first_appear;
    entry = safe = first_appear = NULL;
    int dup_flag = 0;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (first_appear && strcmp(first_appear->value, entry->value) == 0) {
            // duplicated
            dup_flag += 1;
            list_del(&(entry->list));
            q_release_element(entry);
        } else {
            if (dup_flag && first_appear) {
                list_del(&(first_appear->list));
                q_release_element(first_appear);
            }
            dup_flag = 0;
            first_appear = entry;
        }
    }

    if (dup_flag != 0) {
        list_del(&(first_appear->list));
        q_release_element(first_appear);
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    q_reverseK(head, 2);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *node, *safe, *tmp;
    list_for_each_safe (node, safe, head) {
        tmp = node->next;
        node->next = node->prev;
        node->prev = tmp;
    }
    tmp = head->next;
    head->next = head->prev;
    head->prev = tmp;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (k <= 0 || !head || list_empty(head)) {
        return;
    }
    int end = 0;
    struct list_head *fast, *slow;
    fast = head;
    while (1) {
        slow = fast->next;
        for (int i = 0; i < k; ++i) {
            fast = fast->next;
            if (fast == head) {
                end = 1;
                break;
            }
        }
        if (end)
            break;

        // save the position of the subqueue
        struct list_head *before, *after;
        before = slow->prev;
        after = fast->next;

        // cut off the subqueue
        slow->prev = fast;
        fast->next = slow;

        // reverse and reconnect the subqueue
        q_reverse(slow);
        before->next = fast;
        after->prev = slow;
        fast->prev = before;
        slow->next = after;

        // Reposition slow and fast pointers to the correct starting position
        fast = slow;
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }

    int size = q_size(head);
    struct list_head *max_node = NULL, *first_node = head->next, *node = NULL;
    first_node->prev = head->prev;
    head->prev->next = first_node;
    INIT_LIST_HEAD(head);
    for (int i = 0; i < size; i++) {
        max_node = first_node;
        list_for_each (node, first_node) {
            if (descend) {
                if (strcmp(list_entry(max_node, element_t, list)->value,
                           list_entry(node, element_t, list)->value) < 0)
                    max_node = node;
            } else {
                if (strcmp(list_entry(max_node, element_t, list)->value,
                           list_entry(node, element_t, list)->value) <= 0)
                    max_node = node;
            }
        }

        struct list_head *next_node = first_node->next;
        list_del_init(max_node);
        if (max_node == first_node) {
            first_node = next_node;
        }

        if (descend) {
            max_node->next = head;
            max_node->prev = head->prev;
            head->prev->next = max_node;
            head->prev = max_node;
        } else {
            max_node->prev = head;
            max_node->next = head->next;
            head->next->prev = max_node;
            head->next = max_node;
        }
    }
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return 1;

    element_t *entry, *safe;
    struct list_head *stack_head;
    entry = safe = NULL;
    stack_head = head;
    list_for_each_entry_safe (entry, safe, head, list) {
        while (stack_head != head &&
               strcmp(list_entry(stack_head, element_t, list)->value,
                      entry->value) > 0) {
            struct list_head *cur = stack_head;
            stack_head = cur->prev;
            stack_head->next = NULL;
            q_release_element(list_entry(cur, element_t, list));
        }
        stack_head->next = &entry->list;
        entry->list.prev = stack_head;
        entry->list.next = NULL;
        stack_head = &entry->list;
    }

    stack_head->next = head;
    head->prev = stack_head;

    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return 1;

    element_t *entry, *safe;
    struct list_head *stack_head;
    entry = safe = NULL;
    stack_head = head;
    list_for_each_entry_safe (entry, safe, head, list) {
        while (stack_head != head &&
               strcmp(list_entry(stack_head, element_t, list)->value,
                      entry->value) < 0) {
            struct list_head *cur = stack_head;
            stack_head = cur->prev;
            stack_head->next = NULL;
            q_release_element(list_entry(cur, element_t, list));
        }
        stack_head->next = &entry->list;
        entry->list.prev = stack_head;
        entry->list.next = NULL;
        stack_head = &entry->list;
    }

    stack_head->next = head;
    head->prev = stack_head;

    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head)) {
        return 0;
    }

    int k = 0;  // number of queue
    queue_contex_t *e;
    list_for_each_entry (e, head, chain)
        k += (list_empty(e->q)) ? 0 : 1;

    if (k == 0)
        return 0;

    queue_contex_t *qct1, *qct2;
    qct1 = list_entry(head->next, queue_contex_t, chain);
    if (list_empty(qct1->q)) {
        list_for_each_entry (qct2, head, chain)
            if (!list_empty(qct2->q))
                break;

        qct1->q->next = qct2->q->next;
        qct1->q->prev = qct2->q->prev;
        qct2->q->next->prev = qct1->q;
        qct2->q->prev->next = qct1->q;

        qct1->size = qct2->size;
        qct2->size = 0;
        INIT_LIST_HEAD(qct2->q);
        qct1 = qct2 = NULL;
    }

    k--;
    while (k != 0) {
        k >>= 1;
        queue_contex_t *entry = NULL, *qctx1 = NULL, *qctx2 = NULL;
        list_for_each_entry (entry, head, chain) {
            if (!qctx1 && !list_empty(entry->q)) {
                qctx1 = entry;
                continue;
            }
            if (list_empty(entry->q))
                continue;

            qctx2 = entry;


            /* merge */
            struct list_head *l1, *l2, *tmp_head = NULL, *prev = NULL;
            struct list_head **ptr = &tmp_head, **node;
            qctx1->q->prev->next = NULL;
            qctx2->q->prev->next = NULL;
            l1 = qctx1->q->next, l2 = qctx2->q->next;
            for (node = NULL; l1 && l2; *node = (*node)->next) {
                if (descend) {
                    node = (strcmp(list_entry(l1, element_t, list)->value,
                                   list_entry(l2, element_t, list)->value) < 0)
                               ? &l2
                               : &l1;
                } else {
                    node = (strcmp(list_entry(l1, element_t, list)->value,
                                   list_entry(l2, element_t, list)->value) > 0)
                               ? &l2
                               : &l1;
                }
                *ptr = *node;
                (*node)->prev = prev;
                prev = *node;
                ptr = &(*ptr)->next;
            }

            qctx1->q->next = tmp_head;
            tmp_head->prev = qctx1->q;
            if (!l1) {
                *ptr = l2;
                l2->prev = prev;
                qctx1->q->prev = qctx2->q->prev;
                qctx2->q->prev->next = qctx1->q;
                INIT_LIST_HEAD(qctx2->q);
            } else {
                *ptr = l1;
                l1->prev = prev;
                qctx1->q->prev->next = qctx1->q;
                INIT_LIST_HEAD(qctx2->q);
            }

            /* after merge */
            qctx1->size += qctx2->size;
            qctx2->size = 0;
            INIT_LIST_HEAD(qctx2->q);
            qctx1 = qctx2 = NULL;
        }
    }
    queue_contex_t *qctx = list_entry(head->next, queue_contex_t, chain);
    return q_size(qctx->q);
}
