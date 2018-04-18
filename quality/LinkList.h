#include <stdio.h>
#include <stdlib.h>
#include "Order.h"
typedef struct node {
	Order this;
	struct node *next;
}node;

typedef struct LinkList {
	node *head;
}LinkList;

void LinkList_init(LinkList *l);
void LinkList_addOrder(LinkList *l, Order o);
int LinkList_replace(LinkList *l, Order o);
void LinkList_print(LinkList *l);
void LinkList_match(LinkList *buy, LinkList *sell);
void LinkList_delete(LinkList *l, node *del);
void LinkList_deleteHead(LinkList *l);
void LinkList_printTrade(LinkList *buy, LinkList *sell, int quant);
