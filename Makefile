all:
	gcc -Wall -c common.c
	gcc -Wall -c game.c
	gcc -Wall client.c common.o game.o -o client
	gcc -Wall server.c common.o game.o -o server
	
game:
	gcc -Wall -c game.c -o game

clean:
	rm -f client server game game.o common.o