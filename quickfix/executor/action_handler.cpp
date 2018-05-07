#include<sys/time.h>
#include<mysql/mysql.h>
#include<string>
#include<bits/stdc++.h>
#include<iostream>
#include<boost/lexical_cast.hpp>
#include<set>
#include<cstdlib>
#include<iterator>
#include<stdio.h>
#include<ctime>
using namespace std;

#include "action_handler.h"

void check_error(MYSQL * flag,string msg){
		if(!flag){
			printf("%s",msg.c_str());
			exit(0);
		}
}

MYSQL* open_DB() {
	MYSQL *connect = mysql_init(NULL);
	check_error(connect, (char *)"MySQL Initialization failed\n");
	connect = mysql_real_connect(connect, "localhost", "root", "shashank" , "cs_matching_engine" ,0, NULL, 0);
	check_error(connect, (char *)"Connection Unsuccessful\n");
	return connect;
}

void close_DB(MYSQL *connect) { mysql_close(connect); }

time_local get_time(int timestamp) {
	time_local t;
	t.hh = timestamp / 3600;
	t.mm = (timestamp / 60);
	t.ss = timestamp % 60;

	return t;
}

orders select_query(string query) {
	MYSQL *connect;
	MYSQL_ROW row;
	connect = open_DB();
	if(mysql_query(connect, query.c_str())) 
		return false;
	row = mysql_fetch_row(mysql_use_result(connect));
	if(row == NULL) {
		return orders();
	}
	close_DB(connect);
	return orders(atoi(row[1]), string(row[2]), string(row[3]), string(row[4]), string(row[5]), string(row[6]), atof(row[7]), atof(row[8]), string(row[9]), string(row[10]));
}

orders modify_query(string query) {
	MYSQL *connect;
	string cmd;
	connect = open_DB();
	orders o = orders();
	if(mysql_query(connect, query.c_str())) 
		o.status = "reject";
	else
		o.status = "pending";
	close_DB(connect);
	return o;
}

orders call_DB(string sqlQuery) {
	string firstWord = sqlQuery.substr(0, sqlQuery.find(" "));
	if(firstWord == "SELECT" or firstWord == "select")
		return select_query(sqlQuery);
		
	else
		return modify_query(sqlQuery);
}

int log_incoming_orders(orders order) {
	    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);

    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
	char *date, *time, *year, *seconds;
	date = (char *)malloc(16);
	year = (char *)malloc(16);
	time = (char *)malloc(16);
	seconds = (char *)malloc(16);
	strftime(date, sizeof(date), "%d_%m", &tstruct);
	strftime(time, sizeof(time), "%H:%M:%S", &tstruct);
	strftime(year, sizeof(year), "%Y", &tstruct);
	strftime(seconds, sizeof(seconds), "%S", &tstruct);

	char *log_file_name;
	log_file_name = (char *)malloc(sizeof(char) * 16);



	sprintf(log_file_name, "log_incoming_%s_%s", date, year);
	sprintf(time, "%s:%s", time, seconds);
	freopen(log_file_name, "a", stdout);


	cout<<" ["<<time<<"] ";
	cout<<" ["<<order.original_orderID<<"] ";
	cout<<"RECEIVED";
	cout<<endl;
	return 0;
}

int log_execution(orders order) {
	time_t now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);

	char *date, *time, *year, *seconds;
	date = (char *)malloc(16);
	year = (char *)malloc(16);
	time = (char *)malloc(16);
	seconds = (char *)malloc(16);

	strftime(date, sizeof(date), "%d_%m", &tstruct);
	strftime(time, sizeof(time), "%H:%M:%S", &tstruct);
	strftime(year, sizeof(year), "%Y", &tstruct);
	strftime(seconds, sizeof(seconds), "%S", &tstruct);
	//return 0;
	char *log_file_name;
	log_file_name = (char *)malloc(sizeof(char) * 32);


	sprintf(log_file_name, "log_execution_%s_%s", date, year);
	//	return 0;
	sprintf(time, "%s:%s", time, seconds);
	freopen(log_file_name, "a", stdout);
	//return 0;

	cout<<"["<<time<<"]";
	cout<<" ["<<order.original_orderID<<"] ";
	cout<<" ["<<order.client_orderID<<"] ";
	cout<<"MATCHED";
	cout<<endl;
	return 0;
}

bool newOrder(struct orders new_order) {
	log_incoming_orders(new_order);	
	orders z;
	unsigned long int sec = time(NULL);
	//insert into order details	
	std::string command = "INSERT INTO order_details VALUES (" + std::to_string(sec) + "," + std::to_string(sec) + ", \"" + new_order.client_orderID + " \", \"" + new_order.client_orderID + " \", \"" + new_order.symbol + "\",\"" + new_order.side + "\", \"" + new_order.type + "\", " + std::to_string(new_order.price) + "," + std::to_string(new_order.quantity) + ", \"PENDING\", NULL)";
	z = call_DB(command);
	//insert into pending orders	
	command = "INSERT INTO pending_orders VALUES (" + std::to_string(sec) + "," + new_order.client_orderID + ", \"" + std::to_string(new_order.quantity)+ "\",\"" + new_order.side + "\"," + std::to_string(new_order.price) + " NULL, NULL)";
	call_DB(command);	
	return true;
}

bool cancelOrder(struct orders cancel_order) {
	struct orders result;
	std::string command;
	log_incoming_orders(cancel_order);
	unsigned long int sec = time(NULL);
	command = "SELECT * FROM order_details WHERE client_orderID = " + cancel_order.original_orderID;
	result = call_DB(command);
	if(result.status != "PENDING") {
		return false;		
	} 	 	
	command = "UPDATE order_details SET updatedOn = " + std::to_string(sec) + ", status = \"CANCELLED\" WHERE order_id = "+ cancel_order.original_orderID;	
	call_DB(command);

	command = "INSERT INTO order_details VALUES (" + std::to_string(sec) + "," + std::to_string(sec) + ", \"" + cancel_order.client_orderID + " \", \"" + cancel_order.client_orderID + " \", \"" + cancel_order.symbol + "\",\"" + cancel_order.side + "\", \"" + cancel_order.type + "\", " + std::to_string(cancel_order.price) + "," + std::to_string(cancel_order.quantity) + ", \"SUCCESSFULLY CANCELLED\", NULL)";
	
	//delete from pending orders
	command = "DELETE FROM pending_orders WHERE order_id = " + cancel_order.original_orderID ;
	call_DB(command);
	return true;
}

bool replaceOrder(struct orders replace_order) { 
	cancelOrder(replace_order);
	newOrder(replace_order);
	return true;
}

bool executionReport(struct orders matched_order1, struct orders matched_order2) {
	unsigned long int sec = time(NULL);
	struct orders buy_order, sell_order;
	if(matched_order1.side == "0"){
		buy_order = matched_order1;
		sell_order = matched_order2;
	}
	else {
		buy_order = matched_order2;
		sell_order = matched_order1;
	}
	double market_price = std::min(buy_order.price, sell_order.price);
	
	std::string command = "INSERT INTO execution_report(timestamp, quantity, buy_order_id, sell_order_id, ticker, price) VALUES (" + std::to_string(sec) + "," + std::to_string(buy_order.quantity) + " ,\"" + buy_order.client_orderID + "\", \"" + sell_order.client_orderID + "\", \"" + buy_order.symbol + "\", " + std::to_string(market_price) + ");";	
	
	command = "UPDATE order_details SET updatedOn = " + std::to_string(sec) + ", status = \"" + buy_order.status + "\" WHERE order_id = "+ buy_order.client_orderID;
	call_DB(command);	
	command = "UPDATE order_details SET updatedOn = " + std::to_string(sec) + ", status = \"" + sell_order.status + "\" WHERE order_id = "+ sell_order.client_orderID;
	call_DB(command);

	command = "UPDATE order_details SET type = \"1\" WHERE type = \"3\" AND side = \"BUY\" AND price < " + std::to_string(market_price) + ";" ;
	call_DB(command);
	command = "UPDATE order_details SET type = \"1\" WHERE type = \"3\" AND side = \"SELL\" AND price > " + std::to_string(market_price) + ";" ;
	call_DB(command);
	command = "UPDATE order_details SET type = \"2\" WHERE type = \"4\" AND side = \"BUY\" AND price < " + std::to_string(market_price) + ";" ;
	call_DB(command);
	command = "UPDATE order_details SET type = \"2\" WHERE type = \"4\" AND side = \"SELL\" AND price > " + std::to_string(market_price) + ";" ;
	call_DB(command);
	return true;
	
	//call Komal's function
}
int main(){
	orders order;
	order.timestamp = 0;
	order.client_orderID = "client order";
	order.original_orderID = "original order";
	order.symbol = "sys";
	order.type = "SELL";
	order.price = 5000;
	order.quantity = 20;
	newOrder(order);
	//executionReport(order);
	return 1;
}