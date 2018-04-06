import MySQLdb, os, sys

val = os.popen("ldd --version")
str = val.read()
if(str.find("Roland McGrath") == -1):
	print "glibc not found. Please install it and try again"
	sys.exit(0)

#Table Creation

db = MySQLdb.connect(host="localhost", user="root",passwd="shashank")
cursor = db.cursor()
sql = 'CREATE DATABASE cs_matching_engine'
cursor.execute(sql)
db.close()

db = MySQLdb.connect(host="localhost", user="root", passwd="shashank", db="cs_matching_engine")
cursor = db.cursor()
sql = " CREATE TABLE execution_report (timestamp BIGINT NOT NULL, trade_id varchar(16) PRIMARY KEY, quantity INT NOT NULL, buy_order_id varchar(16) NOT NULL, sell_order_id varchar(16) NOT NULL, ticker VARCHAR(8) NOT NULL, price INT NOT NULL);"
cursor.execute(sql)

sql = " CREATE TABLE pending_orders ( timestamp BIGINT UNSIGNED NOT NULL, order_id varchar(16) PRIMARY KEY, quantity INT UNSIGNED, order_type VARCHAR(6), price DOUBLE);"
cursor.execute(sql)

sql = " CREATE TABLE order_details (timestamp BIGINT, handlInst VARCHAR(16), client_orderID varchar(16) PRIMARY KEY, symbol VARCHAR(8), side VARCHAR(8), type VARCHAR(8), price DOUBLE NOT NULL, quantity DOUBLE NOT NULL, status varchar(16));"
cursor.execute(sql)

db.close()
