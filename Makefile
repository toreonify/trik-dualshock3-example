build:
	gcc -c libtrik.c -o libtrik.o
	gcc -c main.c -o main.o
	gcc libtrik.o libenjoy/test/libenjoy.o libenjoy/test/libenjoy_linux.o main.o -o manipulator
