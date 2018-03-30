#include <stdio.h>
#include <stdlib.h>

typedef struct {
	char *handlInst;
	char *client_orderID;
	char *symbol;
	char *side;
	char *type;
	double price;
	double quantity;
} new_obj; 

typedef struct {
	char *client_orderID;
	char *symbol;
	char *side;
	double quantity;
} can_obj;

typedef struct {
	char *handlInst;
	char *client_orderID;
	char *original_orderID;
	char *symbol;
	char *side;
	char *type;
	double price;
	double quantity;
} rep_obj;

new_obj func_n(new_obj obj) {
	printf("%s\n", obj.handlInst);
	printf("%s\n", obj.client_orderID);
	printf("%s\n", obj.symbol);
	printf("%s\n", obj.side);
	printf("%s\n", obj.type);
	printf("%lf\n", obj.price);
	printf("%lf\n", obj.quantity);
	
	obj.price = 100.0;
	return obj;
}

can_obj func_c(can_obj obj) {
	printf("%s\n", obj.client_orderID);
	printf("%s\n", obj.symbol);
	printf("%s\n", obj.side);
	printf("%lf\n", obj.quantity);
	return obj;
}

rep_obj func_r(rep_obj obj) {
	printf("%s\n", obj.handlInst);
	printf("%s\n", obj.client_orderID);
	printf("%s\n", obj.original_orderID);
	printf("%s\n", obj.symbol);
	printf("%s\n", obj.side);
	printf("%s\n", obj.type);
	printf("%lf\n", obj.price);
	printf("%lf\n", obj.quantity);
	
	return obj;
}