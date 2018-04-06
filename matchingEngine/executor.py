from __future__ import print_function
import quickfix as fix
import time
import quickfix44 as fix44
import libs.engine as engine

class Executor(fix.Application):
    orderID = 0
    execID = 0

    def onCreate(self, sessionID):
        print('Application - onCreate', sessionID)
        self.targetCompID = sessionID.getTargetCompID().getValue()
        print('target ID:', self.targetCompID)
        self.sessions = {}
        self.sessions[self.targetCompID] = {}
        self.orders = {}
        self.sessions[self.targetCompID]['sessionID'] = sessionID
        self.sessions[self.targetCompID]['connected'] = False
        return

    def onLogon(self, sessionID): 
        print('Application - onLogon', sessionID)
        self.sessionID = sessionID
        self.sessions[self.targetCompID]['connected'] = True
        print(self.sessions)
        return

    def onLogout(self, sessionID): 
        print('Application - onLogout', sessionID)
        self.sessions[self.targetCompID]['connected'] = False
        print(self.sessions)
        return

    def toAdmin(self, message, sessionID): 
        print('Application - toAdmin', sessionID)
        return

    def toApp(self, message, sessionID): 
        print('Application - toApp', sessionID)
        print('message', message, '\n', self.get_header_value(message, fix.MsgType()))
        return

    def fromAdmin(self, message, sessionID): 
        print('Application - fromAdmin', sessionID)
        return

    def fromApp(self, message, sessionID): 
        print('Application - fromApp', sessionID)
        print('Received message:\n', message)
        m_type = self.get_header_value(message, fix.MsgType())
        message_types = {
            fix.MsgType_NewOrderSingle:'new',
            fix.MsgType_OrderCancelRequest:'cancel',
            fix.MsgType_OrderCancelReplaceRequest:'replace',
            fix.MsgType_MarketDataRequest:self.on_market_data_request,
        }
        f = message_types.get(m_type, None)
        if type(f) is str:
            self.request_to_action_handler(f, message)
        elif f is not None:
            f(message)
        return

    def gen_orderID(self):
        self.orderID += 1
        return `self.orderID`

    def gen_execID(self):
        self.execID += 1
        return `self.execID`
    
    def get_header_value(self, message, key):
        message.getHeader().getField(key)
        return key.getValue()

    def get_field_value(self, message, key):
        message.getField(key)
        return key.getValue()
    
    def call_action_handler(self, request_type, message):
        if(request_type == "new"): 
            obj = engine.orders(int(time.time()), message['handlInst'], message['client_orderID'], message['client_orderID'], 
                message['symbol'], message['side'], message['type'], message['price'], message['quantity'])
            engine.new_order(obj) 
        elif(request_type == "replace"): 
            obj = engine.orders(int(time.time()), message['handlInst'], message['client_orderID'], message['original_orderID'], 
                message['symbol'], message['side'], message['type'], message['price'], message['quantity'])
            engine.replace_order(obj) 

        elif(request_type == "cancel"): 
            obj = engine.orders(int(time.time()), 'handlInst', message['client_orderID'], message['client_orderID'], 
                message['symbol'], message['side'], 'delete', 0.00, message['quantity'])
            engine.cancel_order(obj) 
        
        return True
    
    def request_to_action_handler(self, request_type, message):
        """Send a request to action handler and receive the acknowledgement."""
        print('Request type:', request_type)
        message_fields = self.generate_message_fields(request_type)
        message_details = {}
        for k, v in message_fields.items():
            message_details[k] = self.get_field_value(message, v)

        print('message details')
        print(request_type)
        print(message_details)
        # exit()
        orderID = self.gen_orderID()
        self.orders[orderID] = message_details
        is_success = self.call_action_handler(request_type, message_details)
        return is_success, orderID, message_details

    def generate_message_fields(self, message_type):
        message_fields = {
            'client_orderID':fix.ClOrdID(),
            'symbol':fix.Symbol(),
            'side':fix.Side(),            
            'quantity':fix.OrderQty(),
        }
        if message_type in ['new', 'replace']:
            extra_message_fields = {
                'type':fix.OrdType(),
                'price':fix.Price(),
                'handlInst':fix.HandlInst(),
            }
            message_fields.update(extra_message_fields)
            if message_type == 'replace':
                message_fields['original_orderID'] = fix.OrigClOrdID()

        return message_fields        

    def on_market_data_request(self, message):
        print('Market Data REQUEST!')
        message_fields = {
            'md_reqID':fix.MDReqID(),
            'depth':fix.MarketDepth(),
            'subscription_type':fix.SubscriptionRequestType(),
        }
        message_details = {}
        for k, v in message_fields.items():
            message_details[k] = self.get_field_value(message, v)

        message_details['entry_types'] = {}
        message_details['symbols'] = {}
        n_entry_types = self.get_field_value(message, fix.NoMDEntryTypes())
        group = fix44.MarketDataRequest().NoMDEntryTypes()
        for i in range(n_entry_types):
            message.getGroup(i + 1, group)
            message_details['entry_types'][i] = self.get_field_value(group, fix.MDEntryType())
        n_symbols = self.get_field_value(message, fix.NoRelatedSym())
        group = fix44.MarketDataRequest().NoRelatedSym()
        for i in range(n_symbols):
            message.getGroup(i + 1, group)
            message_details['symbols'][i] = self.get_field_value(group, fix.Symbol())
        orderID = self.gen_orderID()
        self.orders[orderID] = message_details
        
        # testing
        self.test_snaps(message_details, full=True)
        self.test_snaps(message_details, full=False)

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

    def send_cancel_replace_reject(self, orderID, orderStatus, details):
        print('Rejecting!')
        reject = self.start_new_message(fix.MsgType_OrderCancelRequest)
        reject_fields = {
            fix.OrderID:orderID,
            fix.OrdStatus:orderStatus,
            fix.OrigClOrdID:details['original_orderID'],
            fix.ClOrdID:details['client_orderID'],
            fix.CxlRejResponseTo:details['request_from'],
        }
        for k, v in reject_fields.items():
            reject.setField(k(v))
        fix.Session.sendToTarget(reject, self.sessionID)
        return reject

    def send_execution_report(self, orderID, orderStatus, execType, details):
        report = self.start_new_message(fix.MsgType_ExecutionReport)
        # required fields FIX4.4
        report_fields = [
            {
                fix.ExecID:self.gen_execID(),
                fix.OrderID:orderID,
                fix.ExecType:execType,
                fix.OrdStatus:orderStatus,
            },
            {
                fix.Symbol:'symbol',
                fix.Side:'side',
                fix.LeavesQty:'leaves_qty',
                fix.CumQty:'cum_qty',
                fix.AvgPx:'avg_px',
            },
        ]
        # additional fields
        order_specific_fields = {
            fix.ExecType_NEW:{
                fix.OrderQty:'quantity',
                fix.Price:'price',
                fix.OrdType:'type',
            },
            fix.ExecType_TRADE:{
                fix.LastPx:'last_px',
                fix.LastQty:'last_qty',
            },
            fix.ExecType_CANCELED :{
                fix.OrigClOrdID:'original_orderID',
                fix.OrderQty:'quantity',
            },
            fix.ExecType_REPLACE:{
                fix.OrigClOrdID:'original_orderID',
                fix.OrderQty:'quantity',            
            },
            fix.ExecType_REJECTED:{
                fix.OrdRejReason:'reject_reason',
            },
        }
        for i in range(len(report_fields)):
            for k, v in report_fields[i].items():
                if i == 0:
                    report.setField(k(v))
                else:
                    report.setField(k(details[v]))
        report.setField(fix.ClOrdID(details['client_orderID']))
        for k, v in order_specific_fields[execType].items():
            report.setField(k(details[v]))
        fix.Session.sendToTarget(report, self.sessionID)        
        return report        

    def send_market_snapshot_full(self, md_order_list, symbol=None, md_reqID=None):
        snapshot = fix44.MarketDataSnapshotFullRefresh()
        if md_reqID:
            snapshot.setField(fix.MDReqID(md_reqID))
        if symbol:
            snapshot.setField(fix.Symbol(symbol))
        group = fix44.MarketDataSnapshotFullRefresh().NoMDEntries()
        group_fields = {
            fix.MDEntryType:'entry_type',
            fix.MDEntryPx:'price',
            fix.Currency:'currency',
            fix.MDEntrySize:'quantity',
            fix.OrderID:'orderID',
        }
        for i in range(len(md_order_list)):
            data = md_order_list[i]
            for k, v in group_fields.items():
                group.setField(k(data[v]))
            snapshot.addGroup(group)
        fix.Session.sendToTarget(snapshot, self.sessionID)
        return snapshot

    def send_market_data_incremental(self, md_update_list, md_reqID=None):
        increment = fix44.MarketDataIncrementalRefresh()
        if md_reqID:
            increment.setField(fix.MDReqID(md_reqID))
        group = fix44.MarketDataIncrementalRefresh().NoMDEntries()   

        group_fields = {
            'default':{
                fix.MDEntryID:'md_entryID',
                fix.MDEntryRefID:'md_entryRefID',
                fix.Symbol:'symbol',
                fix.MDUpdateAction:'update_action',
            }, 
            'action_specific':{
                0:{
                    fix.MDEntryType:'entry_type',
                    fix.MDEntryPx:'price',
                    fix.MDEntrySize:'quantity',
                },
                1:{
                    fix.MDEntryType:'entry_type',
                    fix.MDEntryPx:'price',
                    fix.MDEntrySize:'quantity',
                },
                2:{
                    fix.DeleteReason:'delete_reason',
                },
            },
        }
        for i in range(len(md_update_list)):
            updates = md_update_list[i]
            for k, v in group_fields['default'].items():
                group.setField(k(updates[v]))
            update_action = updates['update_action']
            for k, v in group_fields['action_specific'][update_action].items():
                group.setField(k(updates[v]))
            increment.addGroup(group)
        fix.Session.sendToTarget(increment, self.sessionID)
        return increment    

    def test_replies(self, orderID, orderStatus, execType, message_details, reject=False):
        message_details.update({
            'cum_qty':0,
            'avg_px':0,
            'leaves_qty':message_details['quantity'],
            'last_qty':message_details['quantity'],
        })
        try:
            message_details.update({
                'last_px':message_details['price'],
            })
        except:
            pass
        if reject:
            reject_details = {
                'original_orderID':message_details['original_orderID'],
                'client_orderID':message_details['client_orderID'],
                'request_from':1,
            }
            orderStatus = fix.OrdStatus_REJECTED
            self.send_cancel_replace_reject(orderID, orderStatus, reject_details)
            return
        self.send_execution_report(orderID, orderStatus, execType, message_details)

    def test_snaps(self, message_details, full=False):
        dummy_order_list = [
            {
                'entry_type':3,
                'orderID':'12',
                'price':12.34,
                'quantity':45,
                'currency':'INR',

            },
            {
                'entry_type':0,
                'orderID':'5',
                'price':1002,
                'quantity':335,
                'currency':'INR',
            },
        ]
        inc_dummy = [
                {
                    'md_entryID':'4',
                    'md_entryRefID':'3',
                    'symbol':'snap',
                    'update_action':0,
                    'entry_type':0,
                    'price':32,
                    'quantity':1123,
                    'delete_reason':0,
                }
        ]
        if full:
            self.send_market_snapshot_full(dummy_order_list, message_details['md_reqID'])
        else:
            self.send_market_data_incremental(inc_dummy, message_details['md_reqID'])

def main():
    try:
        settings = fix.SessionSettings('config/executor.cfg')
        application = Executor()
        storeFactory = fix.FileStoreFactory(settings)
        logFactory = fix.FileLogFactory(settings)
        executor = fix.SocketAcceptor(application, storeFactory, settings, logFactory)
        executor.start()
        while True:
            time.sleep(1)
    except (fix.ConfigError, fix.RuntimeError), e:
        print(e)

if __name__ == '__main__':
    main()
