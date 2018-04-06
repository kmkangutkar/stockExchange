from __future__ import print_function
import quickfix as fix
import time
import quickfix44 as fix44


class Client(fix.Application):
    orderID = 0
    mdReqID = 0

    def pretty_print(self, func_name, sessionID):
        print('Client -', func_name, '| sessionID -', sessionID)

    def onCreate(self, sessionID):
        self.sessionID = sessionID
        self.pretty_print('onCreate', sessionID)

    def onLogon(self, sessionID): 
        self.sessionID = sessionID
        self.pretty_print('onLogon', sessionID)

    def onLogout(self, sessionID): 
        self.pretty_print('onLogout', sessionID)
    
    def toAdmin(self, message, sessionID):
        self.pretty_print('toAdmin', sessionID)
       #print('Message:\n', message)

    def toApp(self, message, sessionID): 
        self.pretty_print('toApp', sessionID)

    def fromAdmin(self, message, sessionID): 
        self.pretty_print('fromAdmin', sessionID)
        #print('Message:\n', message)

    def fromApp(self, message, sessionID): 
        self.pretty_print('fromApp', sessionID)
        print('Message:\n', message)
        print('Message type:', self.get_header_value(message, fix.MsgType()))
    
    def get_header_value(self, message, field): 
        key = field
        message.getHeader().getField(key)
        return key.getValue()

    def gen_orderID(self):
        self.orderID += 1
        return `self.orderID`
    
    def gen_mdReqID(self):
        self.mdReqID += 1
        return `self.mdReqID`
    
    def start_new_message(self, msg_type):
        new_message = fix.Message()
        message_fields = [
            fix.BeginString(fix.BeginString_FIX44), 
            fix.MsgType(msg_type), 
            fix.TransactTime()
        ]
        for field in message_fields:
            new_message.getHeader().setField(field)
        return new_message
   
    def new_order(self):
        order_message = self.start_new_message(fix.MsgType_NewOrderSingle)
        message_fields = {
            fix.ClOrdID:self.gen_orderID(),
            fix.HandlInst:fix.HandlInst_MANUAL_ORDER_BEST_EXECUTION,
            fix.Symbol:'SMBL',
            fix.Side:fix.Side_BUY,
            fix.OrdType:fix.OrdType_LIMIT,
            fix.OrderQty:100,
            fix.Price:10,
        }
        for k, v in message_fields.items():
            order_message.setField(k(v))
        print(order_message.toString())
        fix.Session.sendToTarget(order_message, self.sessionID)

    def cancel_order(self, origClOrdID):
        order_message = self.start_new_message(fix.MsgType_OrderCancelRequest)
        message_fields = {
            fix.ClOrdID:self.gen_orderID(),
            fix.OrigClOrdID:origClOrdID,
            fix.Symbol:'SMBL',
            fix.Side:fix.Side_BUY,
            fix.OrderQty:100,
        }
        for k, v in message_fields.items():
            order_message.setField(k(v))
        print(order_message.toString())
        fix.Session.sendToTarget(order_message, self.sessionID)
    
    def replace_order(self, origClOrdID):
        order_message = self.start_new_message(fix.MsgType_OrderCancelReplaceRequest)
        message_fields = {
            fix.ClOrdID:self.gen_orderID(),
            fix.OrigClOrdID:origClOrdID,
            fix.HandlInst:fix.HandlInst_MANUAL_ORDER_BEST_EXECUTION,
            fix.Symbol:'SMBL',
            fix.Side:fix.Side_BUY,
            fix.OrdType:fix.OrdType_LIMIT,
            fix.OrderQty:100,
            fix.Price:5,
        }
        for k, v in message_fields.items():
            order_message.setField(k(v))
        print(order_message.toString())
        fix.Session.sendToTarget(order_message, self.sessionID)

    def market_data_request(self):
        request = fix44.MarketDataRequest()
        request.setField(fix.MDReqID(self.gen_mdReqID()))
        request.setField(fix.MarketDepth(0))
        request.setField(fix.SubscriptionRequestType('0'))

        group = fix44.MarketDataRequest().NoMDEntryTypes()
        group.setField(fix.MDEntryType('0'))
        request.addGroup(group)
        group.setField(fix.MDEntryType('1'))
        request.addGroup(group)

        group = fix44.MarketDataRequest().NoRelatedSym()
        group.setField(fix.Symbol('SNAP'))
        request.addGroup(group)

        fix.Session.sendToTarget(request, self.sessionID)

def main():
    try:
        settings = fix.SessionSettings('config/client.cfg')
        application = Client()
        storeFactory = fix.FileStoreFactory(settings)
        logFactory = fix.FileLogFactory(settings)
        client = fix.SocketInitiator(application, storeFactory, settings, logFactory)
        client.start()
        while True:
            cmd = raw_input()
            if cmd == 'x':
                client.stop()
                break
            if cmd == 'n':
                # new order
                application.new_order()
            if cmd == 'c':
                # cancel order
                application.cancel_order('1')
            if cmd == 'r':
                # replace order
                application.replace_order('1')
            if cmd == 'm':
                # market data request
                application.market_data_request()
            time.sleep(1)
    except (fix.ConfigError, fix.RuntimeError), e:
        print(e)

if __name__ == '__main__':
    main()
