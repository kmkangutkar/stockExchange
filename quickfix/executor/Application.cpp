#ifdef _MSC_VER
  #pragma warning(disable : 4503 4355 4786)
#else
  #include "config.h"
#endif

#include "Application.h"
#include "quickfix/Session.h"
#include "quickfix/fix44/ExecutionReport.h"
#include "quickfix/fix44/OrderCancelReject.h"

void Application::onCreate(const FIX::SessionID& sessionID) {
  std::cout << "onCreate" << sessionID << std::endl;
}

void Application::onLogon(const FIX::SessionID& sessionID) {
  std::cout << "onLogon" << sessionID << std::endl;  
}

void Application::onLogout(const FIX::SessionID& sessionID) {
  std::cout << "onLogout" << sessionID << std::endl;  
}

void Application::toAdmin(FIX::Message& message, const FIX::SessionID& sessionID) {
  std::cout << "toAdmin - " << sessionID << message << std::endl;  
}

void Application::toApp(FIX::Message& message, const FIX::SessionID& sessionID)
throw(FIX::DoNotSend) {
  std::cout << "toApp -" << sessionID << message << std::endl;
}

void Application::fromAdmin(const FIX::Message& message, const FIX::SessionID& sessionID)
throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon) {
  std::cout << "fromAdmin -" << sessionID << message << std::endl;
}

void Application::fromApp(const FIX::Message& message, const FIX::SessionID& sessionID)
throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType) {
  std::cout << "fromApp -" << sessionID << message << std::endl;
  crack(message, sessionID);
}

void Application::onMessage(const FIX44::NewOrderSingle& message, const FIX::SessionID& sessionID) {
  std::cout << "New order: " << sessionID << message << std::endl;
  
  FIX::Symbol symbol;
  FIX::Side side;
  FIX::OrdType ordType;
  FIX::OrderQty orderQty;
  FIX::Price price;
  FIX::ClOrdID clOrdID;
  FIX::StopPx stopPx;

  message.get(symbol);
  message.get(side);
  message.get(ordType);
  message.get(orderQty);
  message.get(clOrdID);

  if ( ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT )
    message.get(stopPx);
  else { 
  message.get(price);
  }

  FIX::OrigClOrdID origClOrdID = (FIX::OrigClOrdID) clOrdID;

  // orders order = new orders();
  orders order;
  order.client_orderID = clOrdID;
  order.original_orderID = origClOrdID;
  order.symbol = symbol;
  order.price = price;
  order.quantity = orderQty;
  order.sessionID = sessionID.toString();
  order.side = side;
  order.status = "new";
  order.type = ordType;


  string message_type = "new";
  int timestamp = 1200;
  order_details details;
  details.sessionID = sessionID;
  details.orderID = genOrderID();
  details.ordStatus = FIX::OrdStatus_FILLED;
  details.symbol = symbol;
  details.side = side;
  details.ordType = ordType;
  details.orderQty = orderQty;
  details.clOrdID = clOrdID;
  details.origClOrdID = origClOrdID;
  details.timestamp = timestamp;
  details.message_type = message_type;
  if ( ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT )
    details.stopPx = stopPx;
  else {
    details.price = price;
  }

  details.order_status = "executed";
  send_execution_report(details);
}

void Application::onMessage(const FIX44::OrderCancelRequest& message, const FIX::SessionID& sessionID) {
  std::cout << "cancel order" << std::endl;

  FIX::Symbol symbol;
  FIX::Side side;
  FIX::OrderQty orderQty;
  FIX::ClOrdID clOrdID;
  FIX::OrigClOrdID origClOrdID;
  double price = 0.0;
  message.get(symbol);
  message.get(side);
  message.get(orderQty);
  message.get(clOrdID);
  message.get(origClOrdID);

  FIX::OrdType ordType = FIX::OrdType(-1);
  string message_type = "cancel";
  int timestamp = 1200;
  order_details details;
  details.sessionID = sessionID;
  details.orderID = genOrderID();
  details.symbol = symbol;
  details.side = side;
  details.ordType = ordType;
  details.orderQty = orderQty;
  details.price = price;
  details.clOrdID = clOrdID;
  details.origClOrdID = origClOrdID;
  details.timestamp = timestamp;
  details.message_type = message_type;

  details.ordStatus = FIX::OrdStatus_CANCELED;
  details.order_status = "rejected";
  send_execution_report(details);
}

void Application::onMessage(const FIX44::OrderCancelReplaceRequest& message, const FIX::SessionID& sessionID) {
  std::cout << "replace order" << std::endl;

  FIX::Symbol symbol;
  FIX::Side side;
  FIX::OrderQty orderQty;
  FIX::ClOrdID clOrdID;
  FIX::OrdType ordType;
  FIX::OrigClOrdID origClOrdID;
  FIX::Price price;

  message.get(symbol);
  message.get(side);
  message.get(orderQty);
  message.get(clOrdID);
  message.get(origClOrdID);
  message.get(ordType);
  message.get(price);

  string message_type = "replace";
  int timestamp = 1200;
  order_details details;
  details.sessionID = sessionID;  
  details.orderID = genOrderID();
  details.ordStatus = FIX::OrdStatus_PENDING_REPLACE;
  details.symbol = symbol;
  details.side = side;
  details.ordType = ordType;
  details.orderQty = orderQty;
  details.price = price;
  details.clOrdID = clOrdID;
  details.origClOrdID = origClOrdID;
  details.timestamp = timestamp;
  details.message_type = message_type;

  details.order_status = "pending";
  send_execution_report(details);
}

void Application::onMessage(const FIX44::MarketDataRequest& message, const FIX::SessionID& sessionID) {
  std::cout << "Market Data Request!" << std::endl;
  
  FIX::MDReqID mdReqID;
  FIX::MarketDepth marketDepth;
  FIX::SubscriptionRequestType subscriptionRequestType;

  message.get(mdReqID);
  message.get(marketDepth);
  message.get(subscriptionRequestType);

  FIX::NoMDEntryTypes noMDEntryTypes;
  message.get(noMDEntryTypes);
  FIX44::MarketDataRequest::NoMDEntryTypes group;
  std::vector<FIX::MDEntryType> mdEntries(noMDEntryTypes);
  for (int i = 0; i < (int)noMDEntryTypes; i++) {
    message.getGroup(i+1, group);
    FIX::MDEntryType mdEntryType;
    group.get(mdEntryType);
    mdEntries[i] = mdEntryType;
  }
  FIX::NoRelatedSym noRelatedSym;
  message.get(noRelatedSym);
  FIX44::MarketDataRequest::NoRelatedSym symGroup;
  std::vector<FIX::Symbol> symbols(noRelatedSym);
  for (int i = 0; i < (int)noRelatedSym; i++) {
    message.getGroup(i+1, symGroup);
    FIX::Symbol symbol;
    symGroup.get(symbol);
    symbols[i] = symbol;
  }

  // std::vector<dummyOrder> dummyOrderList;
  // send_market_data_snapshot_full(dummyOrderList, sessionID);
  
  std::vector<dummyInc> dummyIncList;
  send_market_data_incremental(dummyIncList, sessionID);
}

void Application::send_execution_report(order_details& details){
  string order_status = details.order_status;
  // pending, cancelled, rejected, partial, executed
  FIX::OrdStatus ordStatus;
  FIX::ExecType execType;
  if (order_status == "pending") {
    ordStatus = FIX::OrdStatus_PENDING_NEW;
    execType = FIX::ExecType_PENDING_NEW;
  } else if (order_status == "cancelled") {
    ordStatus = FIX::OrdStatus_CANCELED;
    execType = FIX::ExecType_CANCELED;
  } else if (order_status == "rejected") {
    ordStatus = FIX::OrdStatus_REJECTED;
    execType = FIX::ExecType_REJECTED;
  } else if (order_status == "partial") {
    ordStatus = FIX::OrdStatus_PARTIALLY_FILLED;
    execType = FIX::ExecType_TRADE;
  } else if (order_status == "executed") {
    ordStatus = FIX::OrdStatus_FILLED;
    execType = FIX::ExecType_TRADE;
  }

  FIX44::ExecutionReport executionReport = FIX44::ExecutionReport(
    FIX::OrderID(details.orderID),
    FIX::ExecID(genExecID()),
    FIX::ExecType(execType),
    FIX::OrdStatus(ordStatus),
        
    FIX::Side(details.side),
    //these values should come from database
    FIX::LeavesQty(details.leavesQty),
    FIX::CumQty(details.cumQty),
    FIX::AvgPx(details.avgPx)
  );
  executionReport.set(details.symbol);
  executionReport.set(details.clOrdID);
  
  if (execType == FIX::ExecType_NEW) {
    executionReport.set(details.orderQty);
    executionReport.set(FIX::Price(details.price));
    executionReport.set(FIX::OrdType(details.ordType));
  }
  else if (execType == FIX::ExecType_TRADE) {
    executionReport.set(FIX::LastPx(details.lastPx));
    executionReport.set(FIX::LastQty(details.lastQty));
  }
  else if ((execType == FIX::ExecType_REPLACE) || (execType == FIX::ExecType_CANCELED)) {
    executionReport.set(FIX::OrigClOrdID(details.origClOrdID));
    executionReport.set(FIX::OrderQty(details.orderQty));
  }
  else if (execType == FIX::ExecType_REJECTED) {
    executionReport.set(FIX::OrdRejReason(details.ordRejReason));
  }

  try {
    FIX::Session::sendToTarget(executionReport, details.sessionID);
  }
  catch (FIX::SessionNotFound&) {}
 
}

void Application::send_market_data_snapshot_full(std::vector<dummyOrder>& dummyOrderList, const FIX::SessionID& sessionID) {
  std::cout << "MD Snapshot!" << std::endl;

  FIX44::MarketDataSnapshotFullRefresh mdSnapshot = FIX44::MarketDataSnapshotFullRefresh();

  int n_entries = sizeof(dummyOrderList) / sizeof(dummyOrderList[0]);
  mdSnapshot.set(FIX::NoMDEntries(n_entries));
  FIX44::MarketDataSnapshotFullRefresh::NoMDEntries group;
  for(int i = 0; i < n_entries; i++) {
    group.set(FIX::MDEntryType(dummyOrderList[i].mdEntryType));
    group.set(FIX::MDEntryPx(dummyOrderList[i].mdEntryPx));
    group.set(FIX::MDEntrySize(dummyOrderList[i].mdEntrySize));
    group.set(FIX::OrderID(dummyOrderList[i].orderID));
    mdSnapshot.addGroup(group);
  }
  try {
    FIX::Session::sendToTarget(mdSnapshot, sessionID);
  }
  catch (FIX::SessionNotFound&) {}

}

void Application::send_market_data_incremental(std::vector<dummyInc>& dummyIncList, const FIX::SessionID& sessionID) {
  std::cout << "MD Incremental!" << std::endl;
  FIX44::MarketDataIncrementalRefresh mdIncrementalRefresh = FIX44::MarketDataIncrementalRefresh();
  int n_entries = sizeof(dummyIncList) / sizeof(dummyIncList[0]);
  mdIncrementalRefresh.set(FIX::NoMDEntries(n_entries));
  FIX44::MarketDataIncrementalRefresh::NoMDEntries group;

  for(int i = 0; i < n_entries; i++) {

    group.set(FIX::Symbol(dummyIncList[i].symbol));
    group.set(FIX::MDEntryID(dummyIncList[i].mdEntryID));
    group.set(FIX::MDEntryRefID(dummyIncList[i].mdEntryRefID));
    group.set(FIX::MDUpdateAction(dummyIncList[i].mdUpdateAction));

    int update_action = dummyIncList[i].mdUpdateAction;
    
    if (update_action == 0 || update_action == 1) {
      group.set(FIX::MDEntryType(dummyIncList[i].mdEntryType));
      group.set(FIX::MDEntryPx(dummyIncList[i].mdEntryPx));
      group.set(FIX::MDEntrySize(dummyIncList[i].mdEntrySize));
    }
    else if (update_action == 2) {
      group.set(FIX::DeleteReason(dummyIncList[i].deleteReason));
    }
    mdIncrementalRefresh.addGroup(group);
  }
  try {
      FIX::Session::sendToTarget(mdIncrementalRefresh, sessionID);
    }
    catch (FIX::SessionNotFound&) {
    }
    
}