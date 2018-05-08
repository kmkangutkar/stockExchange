#include "library.h"

time_local get_time(int timestamp) {
	time_local t;
	t.hh = timestamp / 3600, t.mm = (timestamp / 60), t.ss = timestamp % 60;
	return t;
}

orders newOrder(struct orders new_order) {
	orders z;
	unsigned long int sec = time(NULL);
	string command = "INSERT INTO order_details() VALUES (" + std::to_string(sec) + "," + std::to_string(sec) + ", \"" + new_order.client_orderID + " \", \"" + new_order.original_orderID + " \", \"" + new_order.symbol + "\",\"" + new_order.side + "\", \"" + new_order.type + "\", " + std::to_string(new_order.price) + "," + std::to_string(new_order.quantity) + ", \"PENDING\", \""+ (new_order.sessionID)+"\");";
	z = call_DB(command);
	//printf("Z IS %d",z);
	new_order.timestamp = sec;
	//printf("%d",new_order.timestamp);
	return matchTrade(new_order);
}

bool cancelOrder(struct orders cancel_order) {
	struct orders result;
	std::string command;
//	log_incoming_orders(cancel_order);
	unsigned long int sec = time(NULL);
	command = "SELECT * FROM order_details WHERE client_orderID = " + cancel_order.original_orderID+" AND symbol=\""+cancel_order.symbol+"\"";
	result = call_DB(command);
	if(result.status != "PENDING") {
		return false;		
	} 	 	
	command = "UPDATE order_details SET updatedOn = " + std::to_string(sec) + ", status = \"CANCELLED\" WHERE client_orderID = "+ cancel_order.original_orderID;	
	call_DB(command);

	command = "INSERT INTO order_details() VALUES (" + std::to_string(sec) + "," + std::to_string(sec) + ", \"" + cancel_order.client_orderID + " \", \"" + cancel_order.original_orderID + " \", \"" + cancel_order.symbol + "\",\"" + cancel_order.side + "\", \"" + cancel_order.type + "\", " + std::to_string(cancel_order.price) + "," + std::to_string(cancel_order.quantity) + ", \"SUC.CANCELLED\",\""+cancel_order.sessionID+"\")";
	call_DB(command);
	return true;
}

bool replaceOrder(struct orders replace_order) { 
	cancelOrder(replace_order);
	newOrder(replace_order);
	return true;
}

bool execution_report(struct orders matched_order1, struct orders matched_order2) {
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
	
	std::string command = "INSERT INTO execution_report() VALUES (" + std::to_string(sec) + "," + std::to_string(buy_order.quantity) + " ,\"" + buy_order.client_orderID + "\", \"" + sell_order.client_orderID + "\", \"" + buy_order.symbol + "\", " + std::to_string(market_price) + ");";	
	
	command = "UPDATE order_details SET updatedOn = " + std::to_string(sec) + ", status = \"" + buy_order.status + "\" WHERE client_orderID = \""+ buy_order.client_orderID+"\"AND symbol=\""+buy_order.symbol+"\" ";
	call_DB(command);	
	command = "UPDATE order_details SET updatedOn = " + std::to_string(sec) + ", status = \"" + sell_order.status + "\" WHERE client_orderID = \""+ sell_order.client_orderID+"\"AND symbol=\""+buy_order.symbol+"\" ";
	call_DB(command);

	command = "UPDATE order_details SET type = \"1\" WHERE type = \"3\" AND side = \"1\" AND price < " + std::to_string(market_price) + " AND symbol=\""+buy_order.symbol+"\" ;" ;
	call_DB(command);
	command = "UPDATE order_details SET type = \"1\" WHERE type = \"3\" AND side = \"2\" AND price > " + std::to_string(market_price) + " AND symbol=\""+buy_order.symbol+"\" ;" ;
	call_DB(command);
	command = "UPDATE order_details SET type = \"2\" WHERE type = \"4\" AND side = \"1\" AND price < " + std::to_string(market_price) + " AND symbol=\""+buy_order.symbol+"\" ;" ;
	call_DB(command);
	command = "UPDATE order_details SET type = \"2\" WHERE type = \"4\" AND side = \"2\" AND price > " + std::to_string(market_price) + " AND symbol=\""+buy_order.symbol+"\" ;" ;
	call_DB(command);
	return true;
	
	//call Komal's function
}
// int main(){
// 	orders order(1, "2", "2", "tcs", "1", "2", 1000, 40, "PENDING", "sinfid");
// 	newOrder(order);
// 	system("sleep 5");
// 	order= orders(1, "3", "3", "tcs", "1", "2", 1000, 40, "PENDING", "sinfid");
// 	newOrder(order);system("sleep 5");
// 	order= orders(1, "4", "4", "infy", "1", "2", 2000, 50, "PENDING", "sinfid");
// 	newOrder(order);system("sleep 5");
// order= orders(1, "5", "5", "infy", "2", "2", 2000, 25, "PENDING", "sinfid");
// 	newOrder(order);system("sleep 5");
// order= orders(1, "6", "6", "infy", "2", "2", 2000, 25, "PENDING", "sinfid");
// 	newOrder(order);system("sleep 5");
// 	order= orders(1, "7", "7", "amz", "2", "2", 150, 25, "PENDING", "sinfid");
// 	newOrder(order);system("sleep 5");
// 	order= orders(1, "8", "8", "ggl", "2", "2", 150, 25, "PENDING", "sinfid");
// 	newOrder(order);system("sleep 5");	
// 	cancelOrder(order);
// 	order= orders(1, "9", "7", "amz", "2", "1", 150, 55, "PENDING", "sinfid");
// replaceOrder(order);
// //order= orders(1, "10", "10", "amz", "0", "1", 150, 25, "PENDING", "sinfid");
// //	newOrder(order);system("sleep 5");	
// 	//orders order(1, "3", "3", "add", "0", "2", 1000, 20, "PENDING", "sinfid");
// //newOrder(order);
// //order.client_orderID="4";order.price=100;	
// //replaceOrder(order);
// //	std::string command = "SELECT \\* from order_details;";
// //	call_DB(command);
// //	executionReport(order);
// 	return 1;
// }
