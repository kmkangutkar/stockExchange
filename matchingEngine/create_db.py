import MySQLdb, os, sys

val = os.popen("ldd --version")
str = val.read()
if(str.find("Roland McGrath") == -1):
	print "glibc not found. Please install it and try again"
	sys.exit(0)

#Table Creation

db = MySQLdb.connect(host="localhost", user="root",passwd="shashank")
cursor = db1.cursor()
sql = 'CREATE DATABASE cs_matching_engine'
cursor.execute(sql)
db.close()

db = MySQLdb.connect(host="localhost", user="root", passwd="shashank", db="cs_matching_engine")
cursor = db.cursor()
sql = " CREATE TABLE execution_report (timestamp BIGINT NOT NULL, trade_id varchar(16) PRIMARY KEY, quantity INT NOT NULL, buy_order_id varchar(16) NOT NULL, sell_order_id varchar(16) NOT NULL, ticker VARCHAR(8) NOT NULL, price INT NOT NULL);"
cursor.execute(sql)

sql = " CREATE TABLE pending_orders ( timestamp BIGINT UNSIGNED NOT NULL, order_id varchar(16) PRIMARY KEY, quantity INT UNSIGNED, order_type VARCHAR(6), price DOUBLE);"
cursor.execute(sql)

sql = " CREATE TABLE order_details (timestamp BIGINT NOT NULL, order_id varchar(16) PRIMARY KEY, ticker VARCHAR(8) NOT NULL, order_type VARCHAR(8) NOT NULL, price DOUBLE NOT NULL, quantity DOUBLE NOT NULL, status VARCHAR(16) NOT NULL, broker_id VARCHAR(16) NOT NULL, matched_order_id varchar(16));"
cursor.execute(sql)

db.close()
