CC = gcc

all: test

test: parser.h libparser.a test.c
	$(CC) -o test test.c -iquote ./ -L ./ -lparser
	
