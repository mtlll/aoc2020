#include <stdlib.h>
#include "list.h"

void init_list(list_t *list)
{
	list->next = list;
	list->prev = list;
}
static void __list_add(list_t *new, list_t *prev, list_t *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

void list_add(list_t *new, list_t *head)
{
	__list_add(new, head, head->next);
}

void list_add_tail(list_t *new, list_t *head)
{
	__list_add(new, head->prev, head);
}

static void __list_del(list_t *prev, list_t *next)
{
	next->prev = prev;
	prev->next = next;
}

void list_del(list_t *entry)
{
	__list_del(entry->prev, entry->next);
	init_list(entry);
}

void list_replace(list_t *old, list_t *new)
{
	new->next = old->next;
	new->next->prev = new;
	new->prev = old->prev;
	new->prev->next = new;
}

bool list_empty(list_t *head)
{
	return (head->next == head);
}

bool list_singular(list_t *head)
{
	return !list_empty(head) && (head->next == head->prev);
}

static void
__list_cut_position(list_t *list, list_t *head, list_t *entry)
{
	list_t *new_first = entry->next;
	list->next = head->next;
	list->next->prev = list;
	list->prev = entry;
	entry->next = list;
	head->next = new_first;
	new_first->prev = head;
}

void list_cut_position(list_t *list, list_t *head, list_t *entry)
{
	if (list_empty(head)) {
		return;
	} else if (list_singular(head) && head->next != entry && head != entry) {
		return;
	} else if (entry == head) {
		init_list(list);
	} else {
		__list_cut_position(list, head, entry);
	}
}
void list_cut_position_dirty(list_t *list, list_t *head, list_t *entry)
{
	hlist_cut_position((hlist_t *) list, (hlist_t *) head, (hlist_t *) entry);
	list->prev = entry;
}
static void __list_splice(list_t *list, list_t *prev, list_t *next)
{
	list_t *first = list->next;
	list_t *last = list->prev;

	first->prev = prev;
	prev->next = first;

	last->next = next;
	next->prev = last;
}
void list_splice(list_t *list, list_t *head)
{
	if (!list_empty(list)) {
		__list_splice(list, head, head->next);
	}
}

void list_splice_tail(list_t *list, list_t *head)
{
	if (!list_empty(list)) {
		__list_splice(list, head->prev, head);
	}
}
void list_splice_init(list_t *list, list_t *head)
{
	if (!list_empty(list)) {
		__list_splice(list, head, head->next);
		init_list(list);
	}
}

void list_splice_dirty(list_t *list, list_t *head)
{
	hlist_splice((hlist_t *) list, (hlist_t *) list->prev, (hlist_t *) head);
}

void list_repair(list_t *head)
{
	list_t *cur, *n;
	list_for_each_safe(cur, n, head) {
		n->prev = cur;
	}
}

void hlist_cut_position(hlist_t *list, hlist_t *head, hlist_t *entry)
{
	list->next = head->next;
	head->next = entry->next;
	entry->next = list;
}

static void __hlist_splice(hlist_t *list, hlist_t *last, hlist_t *prev, hlist_t *next)
{
	hlist_t *first = list->next;
	prev->next = first;
	last->next = next;
}

void hlist_splice(hlist_t *list, hlist_t *last, hlist_t *head)
{
	__hlist_splice(list, last, head, head->next);
}
