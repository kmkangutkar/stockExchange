python/mocks/fix

vi fixmock.ini
vi fixmock.py

exchangemocks/

########################################

vi fixmock.ini 
# based on quickfix
# thirdparty/quickfix-VERSION/spec/FIX42.xml

[DEFAULT]
ConnectionType=acceptor
SocketAcceptPort=40991
SocketReuseAddress=Y
FileStorePath=store
UseLocalTime=Y

[SESSION]
BeginString=FIX4.2
SenderCompID=EXCH
TargetCompID=CLIENT
UseDataDictionary=N
DataDictionary=../../thirdparty/quickfix-VERIFIX/spec/FIX42.xml
StartTime=00:00:00
EndTime=23:59:00
CheckCompID=N

########################################

vi fixmock.py

import sys
import time
import quickfix as fix
from collections import deque
import threading

forward_path_queue = deque([])
heartbeatingFlag = 0
# Start exch after the heartbeat with line handler
t_event = threading.Event()

# event for triggering pass or fail status of test case at client side

validation_failed = threading.Event()

class Application(fix.Application)
	orderID = 0
	execID = 0
	mock_queue = deque([])
	
	def onCreate(self, sessionID):
		## Log "Application::onCreate"
		return
	
	def onLogon(self, sessionID):
		## Log "Application::onLogon"
		heartbeatingFlag = 1
		t_event.set()
		t_event.clear()
		return
	
	def onLogout(self, sessionID):
		## Log "Application::onLogout"
		heartbeatingFlag = 0
		return

	def toAdmin(self, message, sessionID):
		## Log "Application::toAdmin"
		return

	def fromAdmin(self, message, sessionID):
		## Log "Application::fromAdmin and str(message)"
		return		

	def toApp(self, message, sessionID):
		## Log "Application::toApp and str(message)"
		return

	def fromApp(self, message, sessionID):
		## Log "Application::fromApp and str(message)"
		msgType = fix.MsgType()
		message.getHeader().getField(msgType)
		switcher = {
			str(fix.MsgType_NewOrderSingle) : self.onNOS
			str(fix.MsgType_OrderCancelRequest) : self.onCXL
			str(fix.MsgType_OrderCancelReplaceRequest) : self.onAmend
			str(fix.MsgType_ExecutionReport) : self.onExecutionRep
		}
		fun = switcher.get(msgType.getValue(), None)
		if func != None:
			func(message) # assumed that onNOS etc will append to self.forward_path_queue
		return

	def appendToMockQueue(self, message):
		self.mock_queue.append(message)
		return

	def onExecutionRep(self, message):
		assert len(forward_path_queue) != 0
		validation_lambda = forward_path_queue.popleft()
		validation_lambda(message)
		queue_len = len(forward_path_queue)
		
		while queue_len > 0:
			executionReport = forward_path_queue.popleft()
			
			# Fetch order id from tag 11
			clOrdID = message.getField(11)
			message.getField(clOrdID)
			executionReport.setField(fix.CLOrdID(clOrdID))
			clOrdID = fix.ClOrdID()
			
			self.appendToMockQueue(executionReport)
			queue_len = queue_len-1
		return
		
	def onNOS(self, message):
		assert len(forward_path_queue) != 0
		validation_lambda = forward_path_queue.popleft()
		validation_lambda(message)
		queue_len = len(forward_path_queue)
		
		while queue_len > 0:
			executionReport = forward_path_queue.popleft()
			
			clOrdID = fix.ClOrdID()
			message.getField(clOrdID)
			executionReport.setField(clOrdID)
			
			self.appendToMockQueue(executionReport)
			queue_len = queue_len-1
		return
	
	def onCXL(self, message):
		assert len(forward_path_queue) != 0
		validation_lambda = forward_path_queue.popleft()
		validation_lambda(message)
		queue_len = len(forward_path_queue)
		
		while queue_len > 0:
			executionReport = forward_path_queue.popleft()
			
			clOrdID = fix.ClOrdID()
			origClOrdId = fix.OrigClOrdID()

			message.getField(clOrdID)
			message.getField(origClOrdId)
			
			executionReport.setField(clOrdID)
			executionReport.setField(origClOrdId)
			
			self.appendToMockQueue(executionReport)
			queue_len = queue_len-1
		return


	def onAmend(self, message):
		assert len(forward_path_queue) != 0
		validation_lambda = forward_path_queue.popleft()
		validation_lambda(message)
		queue_len = len(forward_path_queue)
		
		while queue_len > 0:
			executionReport = forward_path_queue.popleft()
			
			clOrdID = fix.ClOrdID()
			origClOrdId = fix.OrigClOrdID()

			message.getField(clOrdID)
			message.getField(origClOrdId)
			
			executionReport.setField(clOrdID)
			executionReport.setField(origClOrdId)
			
			self.appendToMockQueue(executionReport)
			queue_len = queue_len-1
		return

	def lambda_validation(self, message):
		statusFlag = False
		exception = None
		msg_type = message.getHeader().getField(fix.MsgType().getField())
		
		try:
			assert_len(forward_path_queue) != 0
			validation_info = forward_path_queue.popleft()
			validation_lambda = validation_info[0]
			validation_lambda(message, validation_info[1])
		except AssertionError as exception:
			## Log "AssertionError in >>>> %r %e"
		return msg_type, statusFlag, exception

### Similarly write it for exchangemocks

#################################################

# messages directory will contain all messages

vi nos.py cxl_order.py amend_order.py


vi nos.py

# Below two will be our module contain common enums and NOS FIX structure/fields
import common_enums
import new_order_single

# capture all mandatory fields.
def default_nos():
	nos = new_order_single.NewOrderSingle()
	nos.account = 'COEP'
	nos.cl_ord_id = '1111111'
	nos.currency = 'INR'
	nos.exchange_destination = ''
	nos.price = 10001 # 100.01
	nos.price_exponent = -2 # price factor
	
	# capture all mandatory hearder fields
	nos.standard_header.deliver_to_comp_id = ''
	nos.standard_header.msg_seq_num = 0
	nos.standard_header.on_behalf_comp_id = 'COEPTESTING'
	nos.standard_header.orig_sending_time = 0
	nos.standard_header.sender_comp_id = 'COEP'
	nos.standard_header.sender_sub_id = 'COEP_DEV_1'
	nos.standard_header.sending_time = 0
	nos.standard_header.target_comp_id = 'COEPExch'
	nos.standard_header.target_sub_id = 'EXCH'
	nos.standard_header.last_msg_seq_num_processed = 0

	nos.text = 'default_nos from nos.py'
	nos.settl_currency = 'INR'
	nos.max_show = 0
	nos.max_show_exponent = 0
	nos.transact_time = 0
	
	nos.instrument.security_id = 'PRODUCT1'
	nos.instrument.symbol = ''
	
	nos.order_qty_data.order_qty = 300
	nos.order_qty_data.order_qty_exponent = -1
	
	return nos


## Similarly prepare cxl_order.py amend_order.py

