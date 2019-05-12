build:
	g++ Game.cpp main.cpp -std=c++2a -lsfml-graphics -lsfml-window -lsfml-system

run: build
	./a.out
