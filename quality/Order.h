#define SELL 0
#define BUY 1
#define REPLACE_SELL 2
#define REPLACE_BUY 3

#define FULL 0
#define PARTIAL 1

#define BACKUP 0
#define REALTIME 1

#define PENDING 0
#define PARTIAL 1
#define FILLED 2

#define NOMATCH -1

int state, trade_id, timestamp;
char in_file[128], log_file[128], match_file[128];

typedef struct Order {
	int id,
		timestamp,
		quant,
		price,
		type,
		status,
		broker_id,
		matched_order_id;

	char ch;
}Order;

void Order_setSource(int state);
Order Order_newOrder();
void Order_print(Order *o);
void Order_printVerbose(Order *o);
