all: buildh tree.o main.o link

buildh: 
	g++ tree.h

tree.o: tree.cpp
	g++ tree.cpp -c
	echo tree.cpp compiled

main.o: main.cpp
	g++ main.cpp -c
	echo main.cpp compiled

clean:
	rm *.o

link:
	g++ -Wall -Wextra -fsanitize=address tree.o main.o -o main

