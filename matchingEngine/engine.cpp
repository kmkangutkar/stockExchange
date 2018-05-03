#include <set>
#include <sys/time.h>
#include <mysql/mysql.h>
#include <cstdlib>
#include <string>
#include <iterator>
#include <iostream>
typedef std::string string;

struct orders {
	int timestamp;
	string client_orderID, original_orderID, symbol, side, type;
	double price, quantity;
	orders(int t = 0, string coID = "", string ooID = "", string sm = "", string sd = "", string ty = "", double p = 0.00, double q = 0.00) : 
		timestamp(t), client_orderID(coID), original_orderID(ooID), symbol(sm), side(sd), type(ty), price(p), quantity(q){}

	inline orders operator- (const orders &op) { return orders(timestamp, client_orderID, original_orderID, symbol, side, type, price, quantity - op.quantity); }
	void print() { 
		printf("timestamp = %d, clOrdID = %s, oOrdID = %s, symbol = %s, side = %s, type = %s price = %lf quantity = %lf\n", 
			timestamp, client_orderID.c_str(), original_orderID.c_str(), symbol.c_str(), side.c_str(), type.c_str(), price, quantity); 
	}
};

order getMatchTrade(orders trade) {
	switch(trade.type) {
		case '1':
			// Market Data, current market price execute;
		case '2':
			//Limit order, buy at no more than particular price, i.e sell_price <= buy_price, execute at sell_price.
		case '3':
			// Stop Order becomes market order at this price.
		case '4':
			//Stop-limit, becomes limit order at the price.

	}	
}

int matchTrade(orders trade) {
	orders mtrade = getMatchTrade(trade);
	if(mtrade.timestamp == 0) 
		insert_ob(trade, "PENDING");

	else if(mtrade.quantity == trade.quantity) {
		insert_ob(trade, "EXECUTED");
		update_status(mtrade, "EXECUTED");
	}
	else if(mtrade.quantity > trade.quantity) {
		update_status(mtrade, "PARTIAL");
		insert_ob(mtrade - trade, "PENDING");
		insert_ob(trade, "EXECUTED");
	}	
	else if(mtrade.quantity < trade.quantity) {
		insert_ob(trade, "PARTIAL");
		insert_ob(trade - mtrade, "PENDING");
		update_status(mtrade, "EXECUTED");	
	}
	return 0;
}
