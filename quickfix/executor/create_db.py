import MySQLdb, os, sys

val = os.popen("ldd --version")
str = val.read()
if(str.find("Roland McGrath") == -1):
	print "glibc not found. Please install it and try again"
	sys.exit(0)

#Table Creation

db = MySQLdb.connect(host="localhost", user="root",passwd="root")
cursor = db.cursor()
sql = 'CREATE DATABASE cs_matching_engine'
cursor.execute(sql)
db.close()

db = MySQLdb.connect(host="localhost", user="root", passwd="root", db="cs_matching_engine")
cursor = db.cursor()
sql = " CREATE TABLE execution_report (timestamp BIGINT NOT NULL, quantity INT NOT NULL, buy_order_id varchar(64) NOT NULL, sell_order_id varchar(64) NOT NULL, ticker VARCHAR(8) NOT NULL, price INT NOT NULL);"
cursor.execute(sql)

sql = " CREATE TABLE order_details (createdOn BIGINT, updatedOn BIGINT, client_orderID varchar(64), original_orderID varchar(64), symbol VARCHAR(8), side VARCHAR(8), type VARCHAR(8), price DOUBLE NOT NULL, quantity DOUBLE NOT NULL, status varchar(16), sessionID varchar(64));"
cursor.execute(sql)

db.close()
