
all: server

server: server.c
	gcc -Wall -Werror -pthread -lrt server.c -o server -lpthread

clean:
	rm -f server

run: server
	./server

.PHONY: all run
