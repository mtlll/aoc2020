//
// Created by mtl on 07/05/2021.
//

#ifndef AOC_LIST_H
#define AOC_LIST_H
#include <stdbool.h>
#include <stddef.h>

#define container_of(ptr, type, member) ({				\
	char *__mptr = (char *)(ptr);					\
	((type *)(__mptr - offsetof(type, member))); })

#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

#define list_last_entry(ptr, type, member) \
	list_entry((ptr)->prev, type, member)

#define list_next_entry(pos, member) \
	list_entry((pos)->member.next, typeof(*(pos)), member)

#define list_prev_entry(pos, member) \
	list_entry((pos)->member.prev, typeof(*(pos)), member)

#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

#define list_for_each_entry(pos, head, member)				\
	for (pos = list_first_entry(head, typeof(*pos), member);	\
	     &pos->member != (head);					\
	     pos = list_next_entry(pos, member))

#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

#define list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = list_first_entry(head, typeof(*pos), member),	\
		n = list_next_entry(pos, member);			\
	     &pos->member != (head); 					\
	     pos = n, n = list_next_entry(n, member))

typedef struct list {
    struct list *next, *prev;
} list_t;

typedef struct hlist {
    struct hlist *next;
} hlist_t;

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list name = LIST_HEAD_INIT(name)

void init_list(list_t *list);
void list_add(list_t *new, list_t *head);
void list_add_tail(list_t *new, list_t *head);
void list_del(list_t *entry);
void list_replace(list_t *old, list_t *new);

bool list_empty(list_t *head);
bool list_singular(list_t *head);

void list_cut_position(list_t *list, list_t *head, list_t *entry);
void list_cut_position_dirty(list_t *list, list_t *head, list_t *entry);
void list_splice(list_t *list, list_t *head);
void list_splice_tail(list_t *list, list_t *head);
void list_splice_init(list_t *list, list_t *head);
void list_splice_dirty(list_t *list, list_t *head);

void list_repair(list_t *head);

void hlist_cut_position(hlist_t *list, hlist_t *head, hlist_t *entry);
void hlist_splice(hlist_t *list, hlist_t *last, hlist_t *head);
#endif //AOC_LIST_H
