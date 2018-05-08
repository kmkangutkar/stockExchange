import MySQLdb, os, sys

val = os.popen("ldd --version")
str = val.read()
if(str.find("Roland McGrath") == -1):
	print "glibc not found. Please install it and try again"
	sys.exit(0)

#Table Creation

db = MySQLdb.connect(host="localhost", user="root",passwd="root")
cursor = db.cursor()
sql = 'DROP DATABASE cs_matching_engine'
cursor.execute(sql)
db.close()
