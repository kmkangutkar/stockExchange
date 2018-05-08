#include "library.h"

orders getMatchTrade(orders trade) {
	if(!trade.type.compare("1")) {
		if(atoi(trade.side.c_str()) == 1) return call_DB("SELECT * FROM order_details WHERE side = \"2\" AND upatedOn>="+ std::to_string(trade.timestamp)+ "AND symbol=\""+trade.symbol+"\" AND status=\"PENDING\" ORDER BY price ASC, updatedOn ASC LIMIT 1;");			
		else if(atoi(trade.side.c_str()) == 2) return call_DB("SELECT * FROM order_details WHERE side = \"1\" AND updatedOn>="+ std::to_string(trade.timestamp)+" AND symbol=\""+trade.symbol+"\" AND status=\"PENDING\" ORDER BY price DESC, updatedOn ASC LIMIT 1;");
	}
	if(!trade.type.compare("2")) {
		if(atoi(trade.side.c_str()) == 1) return call_DB("SELECT * FROM order_details WHERE side = \"2\" AND price <=" + std::to_string(trade.price) + " AND updatedOn<="+ std::to_string(trade.timestamp)+"  AND symbol=\""+trade.symbol+"\" AND status=\"PENDING\" ORDER BY price ASC, updatedOn  ASC LIMIT 1;");
		else if(atoi(trade.side.c_str()) == 2) return call_DB("SELECT * FROM order_details WHERE side = \"1\" AND price >=" + std::to_string(trade.price) +" AND updatedOn<="+ std::to_string(trade.timestamp)+" AND symbol=\""+trade.symbol+"\" AND status=\"PENDING\" ORDER BY price DESC, updatedOn ASC LIMIT 1;");
	}	
}
orders matchTrade(orders trade) {
	string command = "";
	orders mtrade = getMatchTrade(trade), ntrade;
unsigned long int sec = time(NULL);
	//mtrade.print();
//trade.print();
	if(!mtrade.timestamp) return trade;
	if(fabs(mtrade.quantity - trade.quantity) < 0.05) {
		//printf("getm 1\n");
		mtrade.status = trade.status = "EXECUTED";
	}
	else if((mtrade.quantity - trade.quantity) > 0) {
		//printf("getm 2\n");
		ntrade = mtrade - trade;		
		mtrade.status = "PARTIAL";
		trade.status = "EXECUTED";
		ntrade.status="PENDING";
		command = "INSERT INTO order_details() VALUES (" + std::to_string(sec) + "," + std::to_string(sec) + ", \"" + ntrade.client_orderID + " \", \"" + ntrade.original_orderID + " \", \"" + ntrade.symbol + "\",\"" + ntrade.side + "\", \"" + ntrade.type + "\", " + std::to_string(ntrade.price) + "," + std::to_string(ntrade.quantity) + ", \"" + ntrade.status + "\", \""+ (ntrade.sessionID)+"\");";
	}	
	else if((mtrade.quantity - trade.quantity) < 0) {
		//printf("getm 3\n");
		ntrade = trade - mtrade;		
		trade.status = "PARTIAL";
		mtrade.status = "EXECUTED";
		ntrade.status="PENDING";
		command = "INSERT INTO order_details() VALUES (" + std::to_string(sec) + "," + std::to_string(sec) + ", \"" + ntrade.client_orderID + " \", \"" + ntrade.original_orderID + " \", \"" + ntrade.symbol + "\",\"" + ntrade.side + "\", \"" + ntrade.type + "\", " + std::to_string(ntrade.price) + "," + std::to_string(ntrade.quantity) + ", \"" + ntrade.status +"\", \""+ (ntrade.sessionID)+"\");";
	}
	execution_report(mtrade, trade);
	if(command.compare("")) call_DB(command);
	trade.price = (min(mtrade.price, trade.price));
	trade.original_orderID = mtrade.client_orderID;
	trade.quantity = ntrade.quantity;
	return trade;//client order orig irder
}
