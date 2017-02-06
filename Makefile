CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -g3

all:			org2vimwiki

org2vimwiki:	org2vimwiki.c
	$(CC) $(CFLAGS) -o $@ $^

test:			org2vimwiki
	mkdir -p tests-output
	./org2vimwiki tests/test1.org tests-output/test1.wiki
	./org2vimwiki tests/aspnes-test.org tests-output/aspnes-test.wiki

clean:
	$(RM) org2vimwiki *.o tests-output
