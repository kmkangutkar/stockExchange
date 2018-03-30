import MySQLdb

val = os.popen("ldd --version")
str = val.read()
if(str.find("This is free software; see the source for copying conditions.  There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. Written by Roland McGrath and Ulrich Drepper.") == -1):
	print "glibc not found. Please install it and try again"
	sys.exit(0)

#Table Creation

db1 = MySQLdb.connect(host="localhost", user="root",passwd="cspune")
cursor = db1.cursor()
sql = 'CREATE DATABASE cs_matching_engine'
cursor.execute(sql)
db1.close()

db = MySQLdb.connect(host="localhost", user="root", passwd="cspune", db="cs_matching_engine")
cursor = db.cursor()
sql = " CREATE TABLE execution_report (timestamp BIGINT NOT NULL, trade_id INT PRIMARY KEY, quantity INT NOT NULL, buy_order_id INT NOT NULL, sell_order_id INT NOT NULL, ticker VARCHAR(10) NOT NULL, price INT NOT NULL);"
cursor.execute(sql)

sql = " CREATE TABLE pending_orders ( timestamp BIGINT UNSIGNED NOT NULL, order_id INT UNSIGNED PRIMARY KEY, quantity INT UNSIGNED, order_type VARCHAR(6), price INT);"
cursor.execute(sql)

sql = " CREATE TABLE order_details (timestamp BIGINT NOT NULL, order_id INT PRIMARY KEY, ticker VARCHAR(10) NOT NULL, order_type VARCHAR(6) NOT NULL, price INT NOT NULL, quantity INT NOT NULL, status VARCHAR(10) NOT NULL, broker_id VARCHAR(10) NOT NULL, matched_order_id INT NOT NULL);"
cursor.execute(sql)

db.close()