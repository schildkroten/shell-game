FLAGS = -Wall -Wextra -fsanitize=address -g

all: game clean

game: main engine gamelib
	mkdir -p bin
	gcc main.o engine.o gamelib.o -o bin/game $(FLAGS)

main:
	gcc main.c -c $(FLAGS)

engine:
	gcc src/engine.c -c $(FLAGS)

gamelib:
	gcc src/gamelib.c -c $(FLAGS)

clean:
	rm -f main.o engine.o gamelib.o
