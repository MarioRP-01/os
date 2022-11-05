CC = gcc

executable = test

target_dir = dist
lib_dir = lib
src_dir = src

libraries = $(wildcard $(lib_dir)/*.h)
sources = $(wildcard $(src_dir)/*.c)
libraries_name = 
sources_name = 

objects = $(target_dir)/

all: $(target_dir) test

$(libraries): 

$(target_dir):
	mkdir $(target_dir)
	
clean:
	rm -r $(target_dir)

# TEST

run: clean_test set_test
	gcc -c test/libreria.c -o test/libreria.o
	ar -rv test/liblibreria.a test/libreria.o
	gcc -o test/test test/test.c -iquote ./test -L ./test -llibreria

.PHONY: run set_test clean_test

set_test: test
	cp lib/* src/* test

test:
	mkdir test

clean_test:
	rm -r test
