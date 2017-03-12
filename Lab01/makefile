all: build_all

build_all:
	g++ -o client2 client2.cpp -std=gnu++11
	g++ -c server2.cpp -o server2.o
	g++ -c libpalindrome.c -o libpalindrome.o
	g++ libpalindrome.o server2.o -o server2
	rm -rf ./*.o


	

