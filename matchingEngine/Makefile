all: packages
	ls
packages:
	echo "Installing dependencies"
	sudo apt-get install python-pip libxml2-dev libmysqlclient-dev mysql-server
	sudo pip2 install quickfix mysqlclient
	cd libs/
	$(MAKE)
	cd ..
db_setup:
	python create_db.py

clean:
	python delete_db.py
	python create_db.py
