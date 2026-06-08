FLAGS = -Wall -g

all: game clean

game: main gamemanager player map
	mkdir -p bin
	gcc main.o gamemanager.o player.o map.o -o bin/game $(FLAGS)

main:
	gcc main.c -c $(FLAGS)

gamemanager:
	gcc src/gamemanager.c -c $(FLAGS)

player:
	gcc src/player.c -c $(FLAGS)

map:
	gcc src/map.c -c $(FLAGS)

clean:
	rm -f main.o gamemanager.o player.o map.o
