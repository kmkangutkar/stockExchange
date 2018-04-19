make

echo "Test 0"

echo "Test 1"
gcc -c Order_main.c
gcc -o input1 Order_main.o

gcc -o 001.out LinkList.o Order.o
./input1 1.in
./001.out 1.in 1.log 1.match

echo "Test 2"
gcc -o 002.out LinkList.o Order.o
./input1 2.log
./002.out 2.log 2.log.check 2.match
diff <(tail -n +2 2.log) 2.log.check

echo "Test 3"

echo "Test 4"
gcc -o 004.out LinkList.o Order.o
gcc -c Order_main_4.c
gcc -o input4 Order_main_4.o

./input4 4.in
./004.out 4.in 4.log 4.match

echo "Test 5"
gcc -o 005.out LinkList.o Order.o
gcc -c Order_main_5.c
gcc -o input5 Order_main_5.o

./input5 5.in
./005.out 5.in 5.log 5.match

echo "Test 6"
gcc -o 006.out LinkList.o Order.o
gcc -c Order_main_6.c
gcc -o input6 Order_main_6.o

./input4 6.in
./006.out 6.in 6.log 6.match

echo "Test 7"
gcc -o 007.out LinkList.o Order.o
gcc -c Order_main_7.c
gcc -o input7 Order_main_7.o

./input4 7.in
./007.out 7.in 7.log 7.match
