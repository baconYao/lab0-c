#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "queue.h"

void _quick_sort(struct list_head *left, struct list_head *right);

struct list_head *_quick_sort_partition(struct list_head *left,
                                        struct list_head *right);

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head) {
        return;
    }

    //  Method 1: list_for_each_entry_safe
    // element_t *item = NULL, *tmp = NULL;
    // list_for_each_entry_safe(item, tmp, head, list) {
    //     free(item->value);
    //     free(item);
    // }

    //  Method 2: list_for_each_safe
    struct list_head *pos, *n;
    list_for_each_safe(pos, n, head) {
        element_t *elem = container_of(pos, element_t, list);
        free(elem->value);
        free(elem);
    }

    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    // 配置 element 記憶體位置
    element_t *new_elem = malloc(sizeof(element_t));
    if (!new_elem)
        return false;

    // 賦予 element 值
    // https://www.geeksforgeeks.org/strdup-strdndup-functions-c/
    new_elem->value = strdup(s);
    if (!new_elem->value) {
        free(new_elem);
        return false;
    }

    // Insert the element to head
    list_add(&new_elem->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s) {
        return false;
    }

    // 配置 element 記憶體位置
    element_t *new_elem = malloc(sizeof(element_t));
    if (!new_elem)
        return false;

    // 賦予 element 值
    // https://www.geeksforgeeks.org/strdup-strdndup-functions-c/
    new_elem->value = strdup(s);
    if (!new_elem->value) {
        free(new_elem);
        return false;
    }

    // Insert the element to tail
    list_add_tail(&new_elem->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    // 若沒有 list 或是 list 是 empty queue
    if (!head || list_empty(head))
        return NULL;

    // Get the element_t containing the first list_head node
    struct list_head *to_remove = head->next;
    element_t *elem = container_of(to_remove, element_t, list);

    /* Copy string to buffer if provided */
    if (sp && bufsize > 0) {
        strncpy(sp, elem->value, bufsize - 1);
        sp[bufsize - 1] = '\0'; /* Ensure null-termination */
    }

    list_del(to_remove);

    /* Isolate the removed node */
    to_remove->next = NULL;
    to_remove->prev = NULL;

    return elem;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    // 若沒有 list 或是 list 是 empty queue
    if (!head || list_empty(head))
        return NULL;


    // Get the element_t containing the end of list_head node
    struct list_head *to_remove = head->prev;
    element_t *elem = container_of(to_remove, element_t, list);

    /* Copy string to buffer if provided */
    if (sp && bufsize > 0) {
        strncpy(sp, elem->value, bufsize - 1);
        sp[bufsize - 1] = '\0'; /* Ensure null-termination */
    }

    list_del_init(to_remove);

    return elem;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each(li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/

    if (!head || list_empty(head))
        return false;

    struct list_head *slow = head->next;
    struct list_head *fast = head->next;


    // 1,2,3,4,5 --> 3
    // 1,2,3,4 --> 3
    // 1,2,3 --> 2
    // 1,2 --> 2
    // 1 --> 1
    while (true) {
        // fast->next != head && fast->next->next != head

        if (fast->next == head) {
            break;
        }
        if (fast->next->next == head) {
            slow = slow->next;
            break;
        }
        slow = slow->next;
        fast = fast->next->next;
    }

    list_del_init(slow);

    element_t *elem = container_of(slow, element_t, list);
    free(elem->value);
    free(elem);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/

    if (!head || list_empty(head))
        return false;

    struct list_head *cur, *next, *check;

    list_for_each_safe(cur, next, head) {
        element_t *elem = list_entry(cur, element_t, list);
        bool has_duplicate = false;  // 表示是否與當前節點是有重複的字串

        check = cur->next;
        while (check != head) {
            element_t *check_elem = list_entry(check, element_t, list);
            if (strcmp(elem->value, check_elem->value) == 0) {
                check = check->next;
                list_del(&check_elem->list);
                free(check_elem->value);
                free(check_elem);
                has_duplicate = true;
            } else {
                check = check->next;
            }
        }

        // 若有其他節點與當前節點重複字串內容
        if (has_duplicate) {
            list_del(&elem->list);
            free(elem->value);
            free(elem);
        }
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    // More elegant way
    struct list_head *curr;
    for (curr = head->next; curr != head && curr->next != head;
         curr = curr->next) {
        struct list_head *next;
        next = curr->next;
        list_move(curr, next);
    }

    // // Intuitive way
    // struct list_head *cur, *next;
    // cur = head->next;
    // while (cur != head && cur->next != head) {
    //     next = cur->next;

    //     cur->prev->next = next;
    //     next->next->prev = cur;

    //     cur->next = next->next;
    //     next->prev = cur->prev;
    //     cur->prev = next;
    //     next->next = cur;

    //     cur = cur->next;
    // }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;


    struct list_head *first_ele, *curr;
    first_ele = head->next;

    for (curr = first_ele->next; curr != head; curr = first_ele->next) {
        list_move(curr, head);
    }
}

// Fixme: 此做法有問題
// /* Reverse the nodes of the list k at a time */
// void q_reverseK(struct list_head *head, int k)
// {
//     if (!head || list_empty(head) || list_is_singular(head))
//         return;

//     struct list_head *start = head->next, *end, *tracker;

//     while (1) {
//         end = start;

//         // 找 k 個節點
//         for (int i = 1; i < k; i++) {
//             end = end->next;
//             if (end == head)
//                 return;  // 不足 k 個
//         }

//         tracker = end->next;

//         // 建立一個暫時的 list head
//         struct list_head tmp;
//         INIT_LIST_HEAD(&tmp);
//         // 把 [head->next .. end] 切出來
//         list_cut_position(&tmp, start, end);
//         // 反轉 tmp list
//         q_reverse(&tmp);
//         // 把反轉後的元素一個個搬回 tracker 之前的節點的後面
//         struct list_head *pos, *n;
//         list_for_each_safe(pos, n, &tmp) {
//             list_move(pos, tracker->prev);
//         }

//         start = tracker;
//     }
// }

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *start = head->next, *end, *tracker;

    while (start != head) {
        end = start;
        // Try to find k elements
        for (int i = 0; i < k - 1; i++) {
            end = end->next;
            if (end == head) {
                return;
            }
        }
        // Update the tracker to be the next start element of linked list
        tracker = end->next;
        // Reverse the [start -> ... -> ... -> end] elements
        struct list_head *curr, *tmp_head = start->prev;
        for (curr = start->next; curr != tracker; curr = start->next) {
            list_move(curr, tmp_head);
        }
        // Update pointer for the next group
        start = tracker;
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    _quick_sort(head->next, head->prev);
    if (descend) {
        q_reverse(head);
    }
}

// sort the link list in ascending
void _quick_sort(struct list_head *left, struct list_head *right)
{
    if (left && left != right && left != right->next) {
        struct list_head *pivot = _quick_sort_partition(left, right);
        _quick_sort(left, pivot->prev);
        _quick_sort(pivot->next, right);
    }
}

struct list_head *_quick_sort_partition(struct list_head *left,
                                        struct list_head *right)
{
    element_t *pivot = list_entry(right, element_t, list);
    struct list_head *partition_index = left;

    for (struct list_head *curr = left; curr != right; curr = curr->next) {
        element_t *elem = list_entry(curr, element_t, list);
        if (strcmp(elem->value, pivot->value) < 0) {
            // swap string pointers
            element_t *ei = list_entry(partition_index, element_t, list);
            char *tmp = ei->value;
            ei->value = elem->value;
            elem->value = tmp;
            // move partition index
            partition_index = partition_index->next;
        }
    }
    // swap string pointers between pivot and partition_index
    element_t *ei = list_entry(partition_index, element_t, list);
    char *tmp = ei->value;
    ei->value = pivot->value;
    pivot->value = tmp;

    return partition_index;
}


/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    if (q_size(head) == 1) {
        return 1;
    }

    struct list_head *cur = head->prev;
    const element_t *elem = list_entry(cur, element_t, list);
    const char *min_str = elem->value;  // min_str 指向的內容不能被修改 (const
                                        // 才能讓靜態分析工具檢查通過)

    // Iterate the list in reverse way
    while (cur->prev != head) {
        element_t *tmp = list_entry(cur->prev, element_t, list);
        if (strcmp(min_str, tmp->value) < 0) {
            list_del(cur->prev);
            free(tmp->value);
            free(tmp);
        } else {
            min_str = tmp->value;
            cur = cur->prev;
        }
    }

    return q_size(head);
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
