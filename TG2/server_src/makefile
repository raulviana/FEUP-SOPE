
all: server

server: server.c
	gcc -Wall -Werror -lpthread -lrt server.c -o server

clean:
	rm -f server

run: server
	./server

.PHONY: all run
