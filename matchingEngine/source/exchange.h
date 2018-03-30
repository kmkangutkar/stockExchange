#include<sys/time.h>
#include<mysql/mysql.h>
#include<bits/stdc++.h>

struct orders {
	int order_id, timestamp, quantity, price;  
	char company;
	orders(int order_id = 0, int timestamp = 0, int quantity = 0, int price = 0, char company = 0): 
		order_id(order_id), timestamp(timestamp), quantity(quantity), price(price), company(company) {};	
	void print() const { 
		printf("order id = %d, timestamp = %d, quantity = %d, price = %d, company = %c\n", order_id, timestamp, quantity, price, company); 
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
