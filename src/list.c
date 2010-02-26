#include "list.h"

/* malloc a new list header and initialize it */
list_t *
list_init(void)
{
	list_t *newlist;

	newlist = (list_t *)malloc(sizeof(list_t));

	newlist->head = NULL;
	newlist->tail = NULL;

	return newlist;
}

/* Add an element to the end of a list */
void
list_add(list_t *list, void *entity)
{
	link_t *newlink = (link_t *)malloc(sizeof(link_t));
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
}

/* Delete an element from the list */
void
list_del_item(list_t *list, void *item)
{
	link_t *templink;

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
}

/* Delete all links and their items */
void
list_del(list_t *list)
{
	link_t *templink;

	while(list->head != NULL)
	{
		templink = list->head;
		list->head = list->head->next;
		if (templink->item != NULL)
			free(templink->item);
		if (templink != NULL)
			free(templink);
	}

	free(list);
}

/* move tail to the head */
void
list_tail_to_head(list_t *list)
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
list_head_to_tail(list_t *list)
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
list_set_list_to_iterate(list_t *list)
{
	if(list)
		g_link = list->head;
}

link_t *
list_next_link()
{
	if(g_link)
		return ((g_link = g_link->next));

	return NULL;
}

link_t *
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

link_t *
list_peek_link()
{
	if(g_link)
		return g_link;

	return NULL;
}
