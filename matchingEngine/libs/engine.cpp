#include "pybind11/pybind11.h"
#include <set>
#include <sys/time.h>
#include <mysql/mysql.h>
#include <string>
#include <cstring>
#include <iterator>
namespace py = pybind11;
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

bool new_order(orders), replace_order(orders), delete_order(orders);

PYBIND11_MODULE(engine, trans) {
	py::class_<orders>(trans, "orders")
		.def(py::init<int, string, string, string, string, string, string, double, double>())
		.def("print", &orders::print);
	trans.def("new_order", &new_order);
	trans.def("replace_order", &replace_order);
	trans.def("delete_order", &delete_order);
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

bool insert_ob(orders) { return 1; }
bool delete_ob(orders) { return 1; }

bool match_order(orders trade) {
	MYSQL *connect;
	string cmd;
	connect = open_DB();
	cmd = command(2, "SELECT client_orderID" , " quantity FROM order_details where symbol = " + fstring(trade.symbol) + " and status = " + fstring("PENDING"))
	if(trade.side == "1")
		cmd = cmd + " and side = " + fstring("2") + " and price <= " + std::to_string(trade.price) + " order by price desc, timestamp asc limit 1";
	else if(trade.side == "2")
		cmd = cmd + " and side = " + fstring("1") + " and price >= " + std::to_string(trade.price) + " order by price asc, timestamp asc limit 1";
	close_DB(connect);
	return true;
}

bool new_order(orders trade) {	
	string cmd;
	MYSQL *connect;
	if(insert_ob(trade)) {
		connect = open_DB();
		cmd = command(9, "INSERT INTO order_details VALUES (" + std::to_string(time(NULL)), fstring(trade.handlInst), fstring(trade.client_orderID), 
				fstring(trade.symbol), fstring(trade.side), fstring(trade.type), std::to_string(trade.price), std::to_string(trade.quantity), fstring("PENDING") + ")");
		if(mysql_query(connect, cmd.c_str())) return false;
		close_DB(connect);
		return true;
	}
	return false;
}

bool replace_order(orders trade) { 
	MYSQL *connect;
	string cmd;
	if(delete_ob(trade) && insert_ob(trade)) {
		connect = open_DB();
		cmd = command(1, "UPDATE order_details set status = " + fstring("REPLACED") +  " where order_id = " + fstring(trade.original_orderID));
		if(mysql_query(connect, cmd.c_str())) return false;
		cmd = command(9, "INSERT INTO order_details VALUES (" + std::to_string(time(NULL)), fstring(trade.handlInst), fstring(trade.client_orderID), 
				fstring(trade.symbol), fstring(trade.side), fstring(trade.type), std::to_string(trade.price), std::to_string(trade.quantity), fstring("PENDING") + ")");
		if(mysql_query (connect, cmd.c_str())) return false;
		return true;
	}
	return false;	
}

bool delete_order(orders trade) {
	MYSQL *connect;
	string cmd;
	if(!delete_ob(trade)) return false;
	cmd = command(2, "UPDATE order_details SET timestamp = " + std::to_string(time(NULL)), 
			" status = " + fstring("CANCELLED") + " WHERE order_id = " + fstring(trade.client_orderID));
	if(mysql_query (connect, cmd.c_str())) return false;
	return true;
}
