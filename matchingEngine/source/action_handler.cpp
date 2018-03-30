#include<sys/time.h>
#include<mysql/mysql.h>
#include<bits/stdc++.h>
#include "exchange.h"
using namespace std;
MYSQL *connect;

int insert_ob(new_obj order) { return 1; }
int insert_ob(rep_obj order) { return 1; }
int delete_ob(can_obj order) { return 1; }
int delete_ob(rep_obj order) { return 1; }

void check_error(MYSQL * flag,string msg){
	if(!flag) {
		printf("%s\n",msg.c_str());
		exit(0);
	}
}

bool new_order(new_obj order) {	
	unsigned long int sec = time(NULL);
	string command;
	if(insert_ob(order)) {	
		command = "INSERT INTO order_details VALUES (" + to_string(sec) + "," + order.client_orderID + ", \"" + order.symbol;
		command	+= "\",\"" + string(order.side) + "\"," + to_string(order.price) + "," + to_string(order.quantity) + ", \"PENDING\", NULL, NULL)";
		if(mysql_query(connect, command.c_str())) return false;
	}
	else return false;
	return true;
}

bool replace_order(rep_obj order) {  
	MYSQL_RES *res;
	MYSQL_ROW row;
	unsigned long int sec = time(NULL);
	string command = "SELECT status FROM order_details WHERE order_id =" + string(order.original_orderID);
	if(mysql_query(connect ,command.c_str())) return false;
	res = mysql_use_result(connect);
	if((row = mysql_fetch_row(res)) == NULL || row[0] != "PENDING" || !delete_ob(order) || !insert_ob(order))
		return false;
	command = "UPDATE order_details set status = \"REPLACED\" where order_id = " + string(order.original_orderID);
	if(mysql_query (connect,command.c_str()))
		return false;
	command = "INSERT INTO order_details VALUES (" + to_string(sec) + "," + string(order.client_orderID) + ", \"" + order.symbol + "\",\"";
	command += string(order.side) + "\"," + to_string(order.price) + "," + to_string(order.quantity) + ", \"PENDING\", NULL, NULL)";
	if(mysql_query (connect,command.c_str()))
		return false;
	return true;
}

bool delete_order(can_obj order) {
	unsigned long int sec = time(NULL);
	if(!delete_ob(order))
		return false;
	string command = "UPDATE order_details SET timestamp = " + to_string(sec) + ", status = \"CANCELLED\" WHERE order_id = "+ order.client_orderID;	
	if(mysql_query (connect,command.c_str()))
		return false;
	return true;
}

int main() {
	connect=mysql_init(NULL);
	check_error(connect, "MySQL Initialization failed\n");
	connect=mysql_real_connect(connect, "localhost", "root", "shashank" , "cs_matching_engine", 0, NULL, 0);
	check_error(connect, "Connection Unsuccessful\n");
	mysql_close(connect);
	return 0;
}
