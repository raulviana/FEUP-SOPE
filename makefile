all: run forensics processFile

forensics: forensics.c
	gcc -Wall -Werror forensics.c -o forensics

processFile: processFile.c
	gcc -Wall -Werror processFile.c -o processFile

clean:
	rm -f forensics processFile

run: forensics
	 ./forensics

.PHONY: all run
