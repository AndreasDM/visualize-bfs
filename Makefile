build:
	g++ *.cpp -Wall -Wextra -std=c++2a -lsfml-graphics -lsfml-window -lsfml-system

run: build
	./a.out
