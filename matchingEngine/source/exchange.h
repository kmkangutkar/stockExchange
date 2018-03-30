#include<sys/time.h>
#include<mysql/mysql.h>
#include<bits/stdc++.h>

typedef struct {
	char *handlInst, *client_orderID, *symbol, *side, *type;
	double price, quantity;
} new_obj; 

typedef struct {
	char *client_orderID, *symbol, *side;
	double quantity;
} can_obj;

typedef struct {
	char *handlInst, *client_orderID, *original_orderID;
	char *symbol, *side, *type;
	double price, quantity;
} rep_obj;

struct orders {
	long long timestamp;
	char *handlInst, *client_orderID, *original_orderID;
	char *symbol, *side, *type;
	double price, quantity;
	void print() const { 
//		printf("order id = %d, timestamp = %d, quantity = %d, price = %d, company = %c\n", order_id, timestamp, quantity, price, company); 
		return;
	}
};

class compareSell {
	public:
		bool operator() (const orders &x, const orders &y) {
			if(x.price == y.price)
				return x.timestamp < y.timestamp;
			else 
				return x.price > y.price; 
		} 
};

class compareBuy {
	public:
		bool operator() (const orders &x, const orders &y) {
			if(x.price == y.price)
				return x.timestamp < y.timestamp;
			else 
				return x.price < y.price; 
		}
};

typedef std::multiset<orders, compareBuy>  buyBook;
typedef std::multiset<orders, compareSell> sellBook;
