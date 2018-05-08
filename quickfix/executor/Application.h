#ifndef EXECUTOR_APPLICATION_H
#define EXECUTOR_APPLICATION_H

#include "quickfix/Application.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/Values.h"
#include "quickfix/Utility.h"
#include "quickfix/Mutex.h"

#include "quickfix/fix44/NewOrderSingle.h"
#include "quickfix/fix44/OrderCancelRequest.h"
#include "quickfix/fix44/OrderCancelReplaceRequest.h"
#include "quickfix/fix44/MarketDataRequest.h"
#include "quickfix/fix44/MarketDataSnapshotFullRefresh.h"
#include "quickfix/fix44/MarketDataIncrementalRefresh.h"

#include "library.h"

typedef std::string string;

typedef struct order_details{
  FIX::OrderID orderID;
  FIX::OrdStatus ordStatus;
  FIX::Symbol symbol;
  FIX::Side side;
  int ordType;
  FIX::OrderQty orderQty;
  FIX::ClOrdID clOrdID;
  FIX::OrigClOrdID origClOrdID;
  int timestamp;
  string message_type;
  FIX::SessionID sessionID;

  //default values
  double price = 0;
  double stopPx = 0;
  FIX::LeavesQty leavesQty = FIX::LeavesQty(0);
  FIX::CumQty cumQty = FIX::CumQty(0);
  FIX::AvgPx avgPx = FIX::AvgPx(0);
  FIX::LastPx lastPx = FIX::LastPx(0);
  FIX::LastQty lastQty = FIX::LastQty(0);
  FIX::OrdRejReason ordRejReason = FIX::OrdRejReason(0);
  FIX::CxlRejResponseTo cxlRejResponseTo = FIX::CxlRejResponseTo(1);
  
  string order_status = "pending";

  order_details(
    FIX::OrderID orderID,
    FIX::OrdStatus ordStatus,
    FIX::Symbol symbol,
    FIX::Side side,
    int ordType,
    FIX::OrderQty orderQty,
    FIX::ClOrdID clOrdID,
    FIX::OrigClOrdID origClOrdID,
    int timestamp
  ) :
    orderID(orderID),
    ordStatus(ordStatus),
    symbol(symbol),
    side(side),
    ordType(ordType),
    orderQty(orderQty),
    clOrdID(clOrdID),
    origClOrdID(origClOrdID),
    timestamp(timestamp)
  {};

}order_details;

// struct orders {
// 	int timestamp;
// 	string client_orderID, original_orderID, symbol, side, type, status;
// 	double price, quantity;

//   //extra
//   string sessionID;
  
// 	orders(int t = 0, string coID = "", string ooID = "", string sm = "", string sd = "", string ty = "", double p = 0.00, double q = 0.00) : 
// 		timestamp(t), client_orderID(coID), original_orderID(ooID), symbol(sm), side(sd), type(ty), price(p), quantity(q){}

// 	inline orders operator- (const orders &op) { return orders(timestamp, client_orderID, original_orderID, symbol, side, type, price, quantity - op.quantity); }
// 	void print() { 
// 		printf("timestamp = %d, clOrdID = %s, oOrdID = %s, symbol = %s, side = %s, type = %s price = %lf quantity = %lf\n", 
// 			timestamp, client_orderID.c_str(), original_orderID.c_str(), symbol.c_str(), side.c_str(), type.c_str(), price, quantity); 
// 	}
// };
typedef struct dummyOrder {
  FIX::MDEntryType mdEntryType = FIX::MDEntryType('0');
  FIX::MDEntryPx mdEntryPx = FIX::MDEntryPx(12.99);
  FIX::MDEntrySize mdEntrySize = FIX::MDEntrySize(100);
  FIX::OrderID orderID = FIX::OrderID("ORDERID");
} dummyOrder;

typedef struct dummyInc {
  FIX::MDEntryID mdEntryID = FIX::MDEntryID("ENTRYID");
  FIX::MDEntryRefID mdEntryRefID = FIX::MDEntryRefID("REFID");
  FIX::Symbol symbol = FIX::Symbol("SNAP");
  FIX::MDUpdateAction mdUpdateAction = FIX::MDUpdateAction(0);
  FIX::MDEntryPx mdEntryPx = FIX::MDEntryPx(32);
  FIX::MDEntrySize mdEntrySize = FIX::MDEntrySize(1100);
  FIX::DeleteReason deleteReason = FIX::DeleteReason(0);
  FIX::MDEntryType mdEntryType = FIX::MDEntryType(0);
} dummyInc;


class Application
: public FIX::Application, public FIX::MessageCracker
{
public:
  Application() : m_orderID(0), m_execID(0) {}

  // Application overloads
  void onCreate( const FIX::SessionID& );
  void onLogon( const FIX::SessionID& sessionID );
  void onLogout( const FIX::SessionID& sessionID );
  void toAdmin( FIX::Message&, const FIX::SessionID& );
  void toApp( FIX::Message&, const FIX::SessionID& )
    throw( FIX::DoNotSend );
  void fromAdmin( const FIX::Message&, const FIX::SessionID& )
    throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon );
  void fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
    throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType );
  
  //user defined
  void send_execution_report(order_details&);
  void send_market_data_snapshot_full(std::vector<dummyOrder>&, const FIX::SessionID&);
  void send_market_data_incremental(std::vector<dummyInc>&, const FIX::SessionID&);
  order_details call_action_handler(struct orders&);

  // MessageCracker overloads
  void onMessage( const FIX44::NewOrderSingle&, const FIX::SessionID& );
  void onMessage( const FIX44::OrderCancelRequest&, const FIX::SessionID& );
  void onMessage(const FIX44::OrderCancelReplaceRequest&, const FIX::SessionID&);
  void onMessage(const FIX44::MarketDataRequest&, const FIX::SessionID&);
  std::string genOrderID() {
    std::stringstream stream;
    stream << ++m_orderID;
    return stream.str();
  }
  std::string genExecID() {
    std::stringstream stream;
    stream << ++m_execID;
    return stream.str();
  }
private:
  int m_orderID, m_execID;
};

#endif
