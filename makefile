all: forensics

forensics: forensics.c
	gcc -Wall -Werror forensics.c -o forensics

clean:
	rm -f forensics

run: forensics
	 ./forensics

.PHONY: all run
