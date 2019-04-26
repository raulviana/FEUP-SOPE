all: client

forensics: client.c
	gcc -Wall -Werror -lpthread -lrt client.c -o client

clean:
	rm -f client

run: client
	./client

.PHONY: all run