make

gcc -c Order_main.c
gcc -o input Order_main.o
./input 1.in

./000.out 1.in 1.log 1.match
