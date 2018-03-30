#include "exchange.h"

MYSQL *connect;
int exec_id=9;

void check_error(MYSQL * flag, std::string msg) {
	if(!flag) { printf("%s", msg.c_str()); exit(0); }
}

void open_DB() {
	connect = mysql_init(NULL);
	check_error(connect, "MySQL Initialization failed\n");
	connect = mysql_real_connect(connect, "localhost", "root", "password" , "Finance" ,0, NULL, 0);
	check_error(connect, "Connection Unsuccessful\n");
}

void close_DB() { mysql_close(connect); }

void insert_db(orders x) {
	
}

void remove_db(orders x) {
	
}

void db_insert(std::string cmd, MYSQL* db) {
	mysql_query(db, cmd.c_str());
}

int match_trade(buyBook::iterator bo, sellBook::iterator so) {
	orders temp;
	if((*bo).price >= (*so).price) {
		temp = ((*bo).quantity < (*so).quantity)? *so: *bo;
		temp.quantity = abs((*so).quantity - (*bo).quantity);
		remove_db(*so), remove_db(*bo);
		if(temp.quantity) insert_db(temp);
	}
	return 0;
}
void start_Engine() {


}
int main() {
	open_DB();
	start_Engine();
	close_DB();
	return 0;
}
