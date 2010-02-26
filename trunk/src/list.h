#ifndef _LIST_H_
#define _LIST_H_

#include <stdlib.h>

typedef struct link_type
{
   struct link_type *next;  /* Next link in the list   */
   struct link_type *prev;  /* Previous link in the list   */
   void             *item;  /* Item owned by this link */
}  link_t;

typedef struct list_type
{
   link_t  *head;           /* First link in the list */
   link_t  *tail;           /* Last link in the list  */
}  list_t;

/* list management */
list_t *list_init(void);
void list_add(list_t *list, void *item);
void list_del_item(list_t *list, void *entity);
void list_del(list_t *list);
void list_tail_to_head(list_t *list);
void list_head_to_tail(list_t *list);

/* list iteration */
link_t *g_link;

void list_set_list_to_iterate(list_t *list);

link_t *list_next_link();
link_t *list_prev_link();
link_t *list_peek_link();

void *list_next_item();
void *list_prev_item();
void *list_peek_item();


#endif /* !_LIST_H_ */
