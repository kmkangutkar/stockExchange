#include <stdio.h>
#include <stdlib.h>
#include "Order.h"
void Order_setSource() {
	if(state == BACKUP) {
		freopen("log.txt", "r", stdin);
		return;
	}
}
Order Order_newOrder() {
	Order this;
	scanf("%d %d %c %d %d %d %d %d %d\n", &(this.timestamp), &(this.id), &(this.ch), &(this.type), &(this.price), &(this.quant), &(this.status), &(this.broker_id), &(this.matched_order_id));


	/* Add random generator here*/
/*	this.ch = 'a';
	this.type = random() % 4;
	this.id = 0;
	this.quant = random() % 5000;
	this.price = random() % 400;
	this.timestamp = 0;
	this.broker_id = random() % 4;
	this.matched_order_id = NOMATCH;
	
	this.status = PENDING;*/
	return this;
}
int Order_sanityCheck(Order *this) {
	if(this->timestamp < 0)
		return 1;
	if(this->id < 0)
		return 2;
	if(this->type < 0 || this->type > 3)
		return 3;
	if(this->price < 0)
		return 4;
	if(this->quant < 0)
		return 5;
	return 0;
}
void Order_print(Order *this) {
	printf("%d %d %c %d %d %d %d %d %d\n", this->timestamp, this->id, this->ch, this->type, this->price, this->quant, this->status, this->broker_id, this->matched_order_id);
}
