all: 
	g++ -O3 -Wall -shared -std=c++11 -fPIC `python -m pybind11 --includes` engine.cpp -o engine.so -I/usr/include/python2.7 -lpython2.7 -lmysqlpp -lmysqlclient
