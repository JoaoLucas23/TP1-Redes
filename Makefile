all:
	gcc -Wall -c common.c
	gcc -Wall -c game.c
	gcc -Wall client.c common.o game.o -o ./bin/client
	gcc -Wall server.c common.o game.o -o ./bin/server
	
game:
	gcc -Wall -c game.c -o game

clean:
	rm -f ./bin/client ./bin/server game game.o common.o