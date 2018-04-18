000.out: LinkList.o Order.o
	gcc -o 000.out LinkList.o Order.o
LinkList.o: Order.h LinkList.h LinkList.c
	gcc -c LinkList.c
Order.o: Order.c Order.h
	gcc -c Order.c
input: Order_main.c
	gcc -c Order_main.c
	gcc -o input Order_main.o
