all: client server

server: server.c
	gcc -Wall -Werror -pthread -lrt server.c -o server -lpthread

client: client.c
	gcc -Wall -Werror -lpthread -lrt client.c -o user