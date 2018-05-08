#include<bits/stdc++.h>
#include <sys/time.h>
#include <mysql/mysql.h>


typedef std::string string;
using namespace std;

typedef struct time_local {
	int hh, mm, ss;
}time_local;

struct orders {
	int timestamp;
	string client_orderID, original_orderID, symbol, side, type, status, sessionID;
	double price, quantity;
	orders(int t = 0, string coID = "", string ooID = "", string sm = "", string sd = "", string ty = "", double p = 0.00, double q = 0.00, string st = "", string sid = "") : 
		timestamp(t), client_orderID(coID), original_orderID(ooID), symbol(sm), side(sd), type(ty), price(p), quantity(q), status(st), sessionID(sid){}

	inline orders operator- (const orders &op) { return orders(timestamp, client_orderID, original_orderID, symbol, side, type, price, quantity - op.quantity); }
	void print() { 
		printf("timestamp = %d, clOrdID = %s, oOrdID = %s, symbol = %s, side = %s, type = %s price = %lf quantity = %lf\n", 
			timestamp, client_orderID.c_str(), original_orderID.c_str(), symbol.c_str(), side.c_str(), type.c_str(), price, quantity); 
	}
};

orders call_DB(string);
orders matchTrade(orders);
bool execution_report(orders, orders);

orders newOrder(struct orders new_order);
bool cancelOrder(struct orders cancel_order);

bool replaceOrder(struct orders replace_order);
