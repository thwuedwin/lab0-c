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
    if (!head)
        return;

    /* free element_t nodes and string inside iteratively */
    element_t *entry = NULL, *safe = NULL;
    list_for_each_entry_safe (entry, safe, head, list) {
        free(entry->value);
        free(entry);
    }

    /* connect head->prev and head->next then free head */
    head->prev->next = head->next;
    head->next->prev = head->prev;
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
    if (k <= 0) {
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
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
