#include "library.h"

void check_error(MYSQL * flag, string msg) { if(!flag) { printf("%s", msg.c_str()); exit(0); } }

MYSQL* open_DB() {
	MYSQL *connect = mysql_init(NULL);
	check_error(connect, (char *)"MySQL Initialization failed\n");
	connect = mysql_real_connect(connect, "localhost", "root", "root" , "cs_matching_engine" ,0, NULL, 0);
	check_error(connect, (char *)"Connection Unsuccessful\n");
	return connect;
}

void close_DB(MYSQL *connect) { mysql_close(connect); }

orders call_DB(string query) {
	cout << query << endl;
	MYSQL *connect; MYSQL_ROW row;
	connect = open_DB();
	if(mysql_query (connect, query.c_str())) return false;
	if(query.find("select") != -1 || query.find("SELECT") != -1){
		row = mysql_fetch_row(mysql_use_result(connect));
		close_DB(connect);
		if(row != NULL) 
{orders(atoi(row[1]), string(row[2]), string(row[3]), string(row[4]), string(row[5]), string(row[6]), atof(row[7]), atof(row[8]), string(row[9]), string(row[10])).print();return orders(atoi(row[1]), string(row[2]), string(row[3]), string(row[4]), string(row[5]), string(row[6]), atof(row[7]), atof(row[8]), string(row[9]), string(row[10]));}
		else return orders();
	}
	close_DB(connect);
	return orders();
}

int main() {
	orders temp(1);
	int offset = 0;
	while(temp.timestamp) {
		temp = call_DB("SELECT * FROM order_details LIMIT " + std::to_string(offset++) + ", 1;");
		if(temp.timestamp) temp.print();
	}		
}
