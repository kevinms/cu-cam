#include <stdlib.h>

#include "list.h"

struct link_t *g_link;

/* malloc a new list header and initialize it */
struct list_t *
list_init(void)
{
	struct list_t *newlist;

	newlist = (struct list_t *)malloc(sizeof(*newlist));

	newlist->head = NULL;
	newlist->tail = NULL;

	newlist->len = 0;

	return newlist;
}

/* Add an element to the end of a list */
void
list_add(struct list_t *list, void *entity)
{
	struct link_t *newlink = (struct link_t *)malloc(sizeof(*newlink));
	newlink->next = NULL;
	newlink->prev = NULL;
	newlink->item = entity;

	if(list->head == NULL) {
		list->head = newlink;
		list->tail = newlink;
	} else {
		newlink->prev = list->tail;
		list->tail->next = newlink;
		list->tail = newlink;
	}
	list->len++;
}

/* Delete an element from the list */
void
list_del_item(struct list_t *list, void *item)
{
	struct link_t *templink;

	if(list == NULL)
		return;

	templink = list->head;
	while(templink != NULL && templink->item != item)
		templink = templink->next;

	if(templink == NULL)
		return;

	if(templink == list->tail && templink == list->head) {
		list->head = NULL;
		list->tail = NULL;
	} else if(templink == list->tail) {
		templink->prev->next = templink->next;
	} else if(templink == list->head) {
		templink->next->prev = templink->prev;
	} else {
		templink->prev->next = templink->next;
		templink->next->prev = templink->prev;
	}

	free(templink);
	list->len--;
}

int list_del_head(struct list_t *list)
{
	struct link_t *tmp;

	if(!list)
		return -1;
	if(!list->head)
		return -1;

	if(list->head->item)
		free(list->head->item);

	tmp = list->head->next;
	free(list->head);

	if(tmp)
		tmp->prev = NULL;

	list->head = tmp;
	return 0;
}

/* Delete all links and their items */
void
list_del(struct list_t *list)
{
	if(!list)
		return;
	while(!list_del_head(list));

	free(list);
}

/* move tail to the head */
void
list_tail_to_head(struct list_t *list)
{
	if(list->tail == list->head)
		return;
	else {
		list->tail->next = list->head;
		list->head->prev = list->tail;
		list->head = list->tail;
		list->tail = list->tail->prev;
		list->head->prev = NULL;
		list->tail->next = NULL;
	}
}

/* move head to the tail */
void
list_head_to_tail(struct list_t *list)
{
	if(list->tail == list->head)
		return;
	else {
		list->head->prev = list->tail;
		list->tail->next = list->head;
		list->tail = list->head;
		list->head = list->head->next;
		list->tail->next = NULL;
		list->head->prev = NULL;
	}
}

void
list_set_list_to_iterate(struct list_t *list)
{
	if(list)
		g_link = list->head;
}

struct link_t *
list_next_link()
{
	if(g_link)
		return ((g_link = g_link->next));

	return NULL;
}

struct link_t *
list_prev_link()
{
	if(g_link)
		return((g_link = g_link->prev));

	return NULL;
}

void *
list_next_item()
{
	if(g_link)
		g_link = g_link->next;
	else
		return NULL;

	if(g_link)
		return g_link->item;

	return NULL;
}

void *
list_prev_item()
{
	if(g_link)
		g_link = g_link->prev;
	else
		return NULL;

	if(g_link)
		return g_link->prev;

	return NULL;
}

void *
list_peek_item()
{
	if(g_link)
		return g_link->item;

	return NULL;
}

struct link_t *
list_peek_link()
{
	if(g_link)
		return g_link;

	return NULL;
}
