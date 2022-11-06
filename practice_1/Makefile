CC = gcc


obj_dir = dist/target
exe_dir = dist
lib_dir = lib
src_dir = src

libraries_name = 
sources_name = 

objects = $(target_dir)/

all: $(target_dir) $(exe_dir)/test

$(exe_dir)/test: $(src_dir)/test 

$(obj_dir):
	mkdir -p $(obj_dir)
	
clean:
	rm -r $(obj_dir)

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
