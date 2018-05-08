CC=g++
CFLAGS=-g -O2 -w -ansi -Wno-unused-command-line-argument -Wpointer-arith -Wwrite-strings -Wno-overloaded-virtual -Wno-deprecated-declarations -Wno-deprecated          -std=c++11 -lmysqlclient
INCLUDE_PATH=-DHAVE_CONFIG_H -I. -I.. -I../include

all: compile

compile: Application.o executor.o action_handler
	echo "compile"
	/bin/bash ../libtool  --tag=CXX   --mode=link $(CC)  $(CFLAGS)   -o executor executor.o Application.o action_handler.o engine.o ignite.o ../src/C++/libquickfix.la  -lpthread       

executor.o:
	echo "executor.o"
	$(CC) $(INCLUDE_PATH)    $(CFLAGS) -MT executor.o -MD -MP -MF .deps/executor.Tpo -c -o executor.o executor.cpp
	mv -f .deps/executor.Tpo .deps/executor.Po

Application.o:
	echo "Application.o"
	$(CC) $(INCLUDE_PATH)    $(CFLAGS) -MT Application.o -MD -MP -MF .deps/Application.Tpo -c -o Application.o Application.cpp
	mv -f .deps/Application.Tpo .deps/Application.Po

action_handler:
	echo "action_handler"
	$(CC) $(INCLUDE_PATH)    $(CFLAGS) -c engine.cpp
	$(CC) $(INCLUDE_PATH)    $(CFLAGS) -c  ignite.cpp
	$(CC) $(INCLUDE_PATH)    $(CFLAGS) -c action_handler.cpp
clean:
	rm -f *.o executor

clean_store:
	rm ../bin/store/*

clean_compile: clean compile

dependencies:
	sudo apt-get install libmysqlclient-dev