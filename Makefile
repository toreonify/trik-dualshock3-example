build:
	gcc -c libtrik.c -o bin/libtrik.o
	gcc -c main.c -o bin/main.o
	gcc bin/libtrik.o libenjoy/test/libenjoy.o libenjoy/test/libenjoy_linux.o bin/main.o -lhgui -o bin/manipulator
