#include<sys/time.h>
#include<mysql/mysql.h>
#include<string>
#include<bits/stdc++.h>
#include<iostream>
#include<boost/lexical_cast.hpp>
using namespace std;
MYSQL *connect;

/*delete_ob() 
	check if original_orderID is -1 , delete client_orderID
	 otherwise delete original_orderID */
/*insert_ob() 
	insert using client_orderID*/
typedef struct no{
	string client_orderID ;
	string symbol;
	int type;
	int side ;
	int quantity;
	float price ;
	int handlInst;
}no;
no new_order;
typedef struct ro{
	string original_clientID ;
	string client_orderID ;
	string symbol ;
	int type;
	int side;
	int quantity;
	float price;
	int handlInst;
}ro;
ro replace_order;
typedef struct co{
	string original_clientID; //=-1
	string client_orderID;
	string symbol;
	int side;
	int quantity;
}co;
co cancel_order;
typedef struct er{
	float timestamp;
	int trade_id;
	int quantity;
	int buy_order_id;
	int sell_order_id;
	string varchar;
	float price;
}er;
typedef struct ob{
	unsigned long int timestamp;
	int order_id;
	int quantity;
	string order_type;
	double price;
}ob;

er execution_report;
int insert_ob(no n);
int insert_ob(ro r);
int delete_ob(ro r);
int delete_ob(co r);
public double queries(string a, string ticker);
public ob order_book(string ticker);

void Barve_ka_function(er parameters);
void check_error(MYSQL * flag,string msg){
		if(!flag){
			printf("%s",msg.c_str());
			exit(0);
		}
}
bool neww(no new_order) {	
	unsigned long int sec = time(NULL);
	if(insert_ob(new_order)){	
		std::string command = "INSERT INTO order_details VALUES (" + boost::lexical_cast<string>(sec) + "," + new_order.client_orderID + ", \"" + new_order.symbol + "\",\"" + boost::lexical_cast<string>(new_order.side) + "\"," + boost::lexical_cast<string>(new_order.price) + "," + boost::lexical_cast<string>(new_order.quantity) + ", \"PENDING\", NULL, NULL)";
		if(mysql_query (connect,command.c_str()))
			return false;
		/*std::string command = "INSERT INTO pending_orders VALUES (" + to_string(sec) + "," + to_string(new_order.client_orderID) + ", \"" + to_string(new_order.quantity)+ "\",\"" + new_order.side + "\"," + to_string(new_order.price) + " NULL, NULL)"
		if(mysql_query (connect,command.c_str()))
			return FALSE*/
	}
	else
		return false;
	return true;
}
bool replace(ro replace_order) {  
	MYSQL_RES *res;
	MYSQL_ROW row;
	unsigned long int sec = time(NULL);
	std::string command = "SELECT status FROM order_details WHERE order_id =" + replace_order.original_clientID;
	if(mysql_query(connect ,command.c_str()))
		return false;
	res = mysql_use_result(connect);
	if((row = mysql_fetch_row(res)) == NULL)
		return false;
	else if(row[0] != "PENDING")
		return false;
	if(!delete_ob(replace_order))
		return false;
	if(!insert_ob(replace_order))
		return false;
	command = "UPDATE order_details set status = \"REPLACED\" where order_id = " + replace_order.original_clientID ;
	if(mysql_query (connect,command.c_str()))
		return false;
	command = "INSERT INTO order_details VALUES (" + boost::lexical_cast<string>(sec) + "," + boost::lexical_cast<string>(new_order.client_orderID) + ", \"" + new_order.symbol + "\",\"" + boost::lexical_cast<string>(new_order.side) + "\"," + boost::lexical_cast<string>(new_order.price) + "," + boost::lexical_cast<string>(new_order.quantity) + ", \"PENDING\", NULL, NULL)";
	if(mysql_query (connect,command.c_str()))
		return false;
	//std::command = "UPDATE pending_orders set timestamp = " + str(timestamp) + ", order_id = " + str(replace_order["client_orderID"]) + ", quantity = " + str(replace_order["quantity"])"\", order_type = \"" + replace_order["side"] + "\", price = " + str(replace_order["price"]) + "where order_id = " + replace_order["original_clientID"]
	//mysql_query (connect,command.c_str());
	return true;
}
bool deletee(co cancel_order) {
/*insert field into cancel order - "original_clientID": -1*/
	unsigned long int sec = time(NULL);
	if(!delete_ob(cancel_order))
		return false;
	std::string command = "UPDATE order_details SET timestamp = " + boost::lexical_cast<string>(sec) + ", status = \"CANCELLED\" WHERE order_id = "+ cancel_order.client_orderID;	
	if(mysql_query (connect,command.c_str()))
		return false;
		//delete from pending orders?
	/*std::string command = "DELETE FROM pending_orders WHERE order_id = " + cancel_order["client_orderID"]
	if(mysql_query (connect,command.c_str()))
		return FALSE*/
	return true;
}
void function_name_can_change(er parameters){
	Barve_ka_function(parameters);
}

public double queries(string a, string ticker) {
	if(a == "last traded price") {
		string command = "SELECT price FROM execution_report WHERE ticker =" + ticker + "ORDER BY" + timestamp + "DESC";
		mysql_query(connect, command.c_str());
		res = mysql_use_result(connect);
		if((row = mysql_fetch_row(res)) == NULL)
			return -98;   //just a failure message
		row = mysql_fetch_row(res);
		return row[0];
	}
	else if(a == "opening day") {
		string command = "SELECT price FROM execution_report WHERE ticker =" + ticker + "ORDER BY" + timestamp + "ASC";	//I've assumed that the exec report contains executions only of that day
		mysql_query(connect, command.c_str());
		res = mysql_use_result(connect);
		if((row = mysql_fetch_row(res)) == NULL)
			return -98;   //just a failure message
		row = mysql_fetch_row(res);
		return row[0];
	}
	//else if(a == "previous day closing price") {
/////////////*******I will add this shortly :( 
	//}
}

public ob* order_book(string ticker) {
	string command = "SELECT timestamp, order_id, quantity, order_type, price FROM order_details WHERE ticker =" + ticker + "AND status = \"PENDING\";
	mysql_query(connect, command.c_str());
	MYSQL_RES *confres = mysql_store_result(sqlhnd);
	int totalrows = mysql_num_rows(confres);
	int numfields = mysql_num_fields(confres);
	static ob orderbk[totalrows];
	int i;
	for(i = 0; i < numfields; i++) {
		orderbk[i].timestamp = 0;
		orderbk[i].order_id = 0;
		orderbk[i].quantity = 0;
		orderbk[i].order_type = " ";
		orderbk[i].price = 0;
	}
	i = 0;
	while((row = mysql_fetch_row(confres))) {
			orderbk[i].timestamp = row[0];
			orderbk[i].order_id = row[1];
			orderbk[i].quantity = row[2];
			orderbk[i].order_type = row[3];
			orderbk[i].price = row[4];
       	       	}
		while(i < totalrows)
			i++;
	}
	return orderbk;
}

int main() {
	connect=mysql_init(NULL);
	check_error(connect, "MySQL Initialization failed\n");
	connect=mysql_real_connect(connect, "localhost", "root", "Bhavana21" , "TESTDB" , 0, NULL, 0);
	check_error(connect, "Connection Unsuccessful\n");
	

	std::string command = "Insert into abcd values ( \"Bhargavi\", 7);";
	if(mysql_query (connect,command.c_str()))
		cout<< "NO";
	mysql_close (connect);
	return 0;
}

