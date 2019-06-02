build:
	g++ *.cpp -Wall -Wextra -O3 -std=c++2a -lsfml-graphics -lsfml-window -lsfml-system

run: build
	./a.out
