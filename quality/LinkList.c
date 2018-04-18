#include "LinkList.h"
#include <string.h>

void LinkList_delete(LinkList *l, node *del) {
	if(del == l->head) {
		l->head = del->next;
		free(del);
		return;
	}
	node *p = l->head;
	while(p && p->next != del) {
		p = p->next;
	}
	p->next = del->next;
	free(del);
}
void LinkList_deleteHead(LinkList *l) {
	if(l->head == NULL)
		return;
	node *p = l->head;
	l->head = p->next;
	free(p);
}
void LinkList_init(LinkList *l) {
	l->head = NULL;
}
void LinkList_addOrder(LinkList *l, Order o) {
	if(l->head == NULL) {
		l->head = (node *)malloc(sizeof(node));
		l->head->this = o;
		l->head->next = NULL;
		return;
	}
	node *lead, *follow;
	lead = follow = l->head;

	if(o.type == BUY) {
		while(lead && lead->this.price >= o.price) {
			lead = lead->next;
		}
	}
	if(o.type == SELL) {
		while(lead && lead->this.price <= o.price) {
			lead = lead->next;
		}
	}
	if(lead == l->head) {
		follow = (node *)malloc(sizeof(node));
		follow->this = o;
		follow->next = lead;
		l->head = follow;
		return;
	}
	while(follow->next != lead) {
		follow = follow->next;
	}
	follow->next = (node *)malloc(sizeof(node));
	follow->next->this = o;
	follow->next->next = lead;
}
void LinkList_printTrade(LinkList *buy, LinkList *sell, int quant) {
	printf("%d %d %d %d %d %c %d\n", timestamp, trade_id++, quant, buy->head->this.id, sell->head->this.id, buy->head->this.ch, buy->head->this.price);
}
void LinkList_match(LinkList *buy, LinkList *sell) {
	if(buy->head == NULL || sell->head == NULL)
		return;
	if(buy->head->this.price >= sell->head->this.price) {
		if(buy->head->this.quant > sell->head->this.quant) {
			buy->head->this.quant -= sell->head->this.quant;
			freopen(match_file, "a", stdout);
			LinkList_printTrade(buy, sell, sell->head->this.quant);
			freopen("/dev/ttys000", "a", stdout);
			sell->head->this.status = FILLED;
			LinkList_deleteHead(sell);
			return;
		}
		if(buy->head->this.quant < sell->head->this.quant) {
			sell->head->this.quant -= buy->head->this.quant;
			freopen(match_file, "a", stdout);
			LinkList_printTrade(buy, sell, buy->head->this.quant);
			freopen("/dev/ttys000", "a", stdout);
			buy->head->this.status = FILLED;
			LinkList_deleteHead(buy);
			return;
		}
		if(buy->head->this.quant == sell->head->this.quant) {
			freopen(match_file, "a", stdout);
			LinkList_printTrade(buy, sell, buy->head->this.quant);
			freopen("/dev/ttys000", "a", stdout);
			sell->head->this.status = FILLED;
			buy->head->this.status = FILLED;
			LinkList_deleteHead(buy);
			LinkList_deleteHead(sell);
			return;
		}
	}
}
int LinkList_replace(LinkList *l, Order o) {
	node *p = l->head;
	while(p && p->this.id != o.id)
		p = p->next;
	if(p == NULL)
		return 1;
	//p->this = o;
	LinkList_delete(l, p);
	if(o.type == REPLACE_BUY)
		o.type = BUY;
	if(o.type == REPLACE_SELL)
		o.type = SELL;
	LinkList_addOrder(l, o);
	return 0;
}
void LinkList_print(LinkList *l) {
	node *p = l->head;
	while(p) {
		Order_print(&(p->this));
		p = p->next;
	}
}

int main(int argc, char *argv[]) {
	Order o;
	strcpy(in_file, argv[1]);
	strcpy(log_file, argv[2]);
	strcpy(match_file, argv[3]);
	char remove_files[12800];
	sprintf(remove_files, "rm -rf %s %s", log_file, match_file);
	system(remove_files);
	state = REALTIME;
	LinkList buy, sell;
	LinkList_init(&buy);
	LinkList_init(&sell);
	timestamp = state = trade_id = 0;
	int t = 0;
	freopen(in_file, "rb", stdin);
	scanf("%d", &t);
	printf("t = %d\n", t);
	while(t--) {
		o = Order_newOrder();
		freopen(log_file, "ab", stdout);
		Order_print(&o);
		if(o.type == BUY)
			LinkList_addOrder(&buy, o);
		if(o.type == SELL)
			LinkList_addOrder(&sell, o);
		if(o.type == REPLACE_BUY)
			LinkList_replace(&buy, o);
		if(o.type == REPLACE_SELL)
			LinkList_replace(&sell, o);
		LinkList_match(&buy, &sell);
		timestamp++;
	}
	/*printf("Buy book...\n");
	LinkList_print(&buy);
	printf("Sell book...\n");
	LinkList_print(&sell);*/
	return 0;
}
