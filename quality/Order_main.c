#include <stdio.h>
#include <stdlib.h>
#include "Order.h"

int main(int argc, char *argv[]) {
	Order *this, o;
	int timestamp = 0;
	freopen(argv[1], "w", stdout);
	int t = 140;
	printf("%d\n", t);
	while(t--) {
	this = &o;
	/* Add random generator here*/
	o.ch = 'a';
	o.type = random() % 4;
	o.quant = random() % 5000;
	o.price = random() % 400;
	o.timestamp = timestamp++;
	o.id = timestamp * 10;
	o.broker_id = random() % 4;
	o.matched_order_id = NOMATCH;
	
	o.status = PENDING;
	printf("%d %d %c %d %d %d %d %d %d\n", this->timestamp, this->id, this->ch, this->type, this->price, this->quant, this->status, this->broker_id, this->matched_order_id);
	}
	return 0;
}
