#include <set>
#include <sys/time.h>
#include <mysql/mysql.h>
#include <cstdlib>
#include <string>
#include <iterator>
#include <iostream>
typedef std::string string;

struct orders {
	orders(int t = 0, string hI = "", string coID = "", string ooID = "", string sm = "", string sd = "", string ty = "", double p = 0.00, double q = 0.00) : 
		timestamp(t), handlInst(hI), client_orderID(coID), original_orderID(ooID), symbol(sm), side(sd), type(ty), price(p), quantity(q){}

	int timestamp;
	string handlInst, client_orderID, original_orderID, symbol, side, type;
	double price, quantity;
	void print() { printf("%d %s %s %s %s %s %s %lf %lf\n", timestamp, handlInst.c_str(), client_orderID.c_str(), original_orderID.c_str(), 
			symbol.c_str(), side.c_str(), type.c_str(), price, quantity); }
};

bool new_order(orders), replace_order(orders), cancel_order(orders);

#include "pybind11/pybind11.h"
PYBIND11_MODULE(engine, trans) {
	namespace py = pybind11;
	py::class_<orders>(trans, "orders")
		.def(py::init<int, string, string, string, string, string, string, double, double>())
		.def("print", &orders::print);
	trans.def("new_order", &new_order);
	trans.def("replace_order", &replace_order);
	trans.def("cancel_order", &cancel_order);
}

string fstring(string s) { return "\"" + s + "\""; }

string command(int num, ...) {
	va_list valist;
	string fcom;
	va_start(valist, num);
	fcom = va_arg(valist, string);
	for(int i = 1; i < num; i++) fcom = fcom + "," + va_arg(valist, string);
	va_end(valist);
	return fcom;
}

void check_error(MYSQL * flag, string msg) { if(!flag) { printf("%s", msg.c_str()); exit(0); } }

MYSQL* open_DB() {
	MYSQL *connect = mysql_init(NULL);
	check_error(connect, (char *)"MySQL Initialization failed\n");
	connect = mysql_real_connect(connect, "localhost", "root", "shashank" , "cs_matching_engine" ,0, NULL, 0);
	check_error(connect, (char *)"Connection Unsuccessful\n");
	return connect;
}

void close_DB(MYSQL *connect) { mysql_close(connect); }

bool insert_ob(orders trade, string status) {
	MYSQL *connect;
	string cmd;
	connect = open_DB();
	cmd = command(11, "INSERT INTO order_details VALUES (" + std::to_string(time(NULL)), std::to_string(time(NULL)), fstring(trade.handlInst), fstring(trade.client_orderID), 
			fstring(trade.original_orderID), fstring(trade.symbol), fstring(trade.side), fstring(trade.type), std::to_string(trade.price), std::to_string(trade.quantity), 
			fstring(status) + ")");
	if(mysql_query(connect, cmd.c_str())) return false;
	close_DB(connect);
	return true;
}

bool delete_ob(orders trade) { 
	MYSQL *connect; MYSQL_ROW row;
	string cmd;
	connect = open_DB();
	cmd = command(1, "SELECT client_orderID FROM order_details where symbol = " + fstring(trade.symbol) + " and status = " + fstring("PENDING") + 
			" and client_orderID = " + fstring(trade.client_orderID));
	if(mysql_query (connect, cmd.c_str())) return false;
	row = mysql_fetch_row(mysql_use_result(connect));
	if(row == NULL) return false;
	cmd = command(2, "UPDATE order_details SET timestamp = " + std::to_string(time(NULL)), 
			" status = " + fstring("CANCELLED") + " WHERE client_orderID = " + fstring(trade.client_orderID));
	if(mysql_query (connect, cmd.c_str())) return false;

	close_DB(connect);
	return true;

}

bool update_status(orders trade, string status) {
	MYSQL *connect;
	string cmd;
	connect = open_DB();
	cmd = command(1, "UPDATE order_details SET status = " + fstring(status) + " ,updatedON = " + std::to_string(time(NULL)) 
			+ " WHERE client_orderID = " + fstring(trade.original_orderID) + " AND status = " + fstring("PENDING"));
	if(mysql_query(connect, cmd.c_str())) return false;
	close_DB(connect);
	return true;
}

orders get_matchTrade(orders trade) {
	MYSQL *connect; MYSQL_ROW row;
	string cmd;
	connect = open_DB();
	
	cmd = command(1, "SELECT * FROM order_details where symbol = " + fstring(trade.symbol) + " and status = " + fstring("PENDING"));
	if(trade.side == "1") cmd += " and side = " + fstring("2") + " and price <= " + std::to_string(trade.price) + " order by price desc, createdON asc limit 1";
	else if(trade.side == "2") cmd += " and side = " + fstring("1") + " and price >= " + std::to_string(trade.price) + " order by price asc, createdON asc limit 1";
	if(mysql_query (connect, cmd.c_str())) return false;
	row = mysql_fetch_row(mysql_use_result(connect));
	if(row == NULL) return orders();
	close_DB(connect);
	return orders(atoi(row[1]), string(row[2]), string(row[3]), string(row[4]), string(row[5]), string(row[6]), string(row[7]), atof(row[8]), atof(row[9]));
}


bool match_trade(orders trade) {
	orders mtrade = get_matchTrade(trade);
	if(mtrade.timestamp == 0)  
		insert_ob(trade, "PENDING");

	else if(mtrade.quantity == trade.quantity) {
		insert_ob(trade, "EXECUTED");
		update_status(mtrade, "EXECUTED");
	}
	else if(mtrade.quantity > trade.quantity) {
		insert_ob(trade, "EXECUTED");
		update_status(mtrade, "PARTIAL");
		mtrade.quantity -= trade.quantity;
		insert_ob(mtrade, "PENDING");
	}	
	else if(mtrade.quantity < trade.quantity) {
		insert_ob(trade, "PARTIAL");
		trade.quantity -= mtrade.quantity;
		insert_ob(trade, "PENDING");
		update_status(trade, "EXECUTED");	
	}
	return true;
}

bool new_order(orders trade) { return match_trade(trade); }
bool cancel_order(orders trade) { return delete_ob(trade); }
bool replace_order(orders trade) { return cancel_order(trade)? new_order(trade): false; }
