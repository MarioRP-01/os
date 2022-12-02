all: myshell

myshell: myshell.c libparser.a parser.h
	gcc -Wall -Wextra myshell.c libparser.a -o myshell -static

debug: myshell.c libparser.a parser.h
	gcc -g -Wall -Wextra myshell.c libparser.a -static

clear: myshell a.out
	rm myshell a.out