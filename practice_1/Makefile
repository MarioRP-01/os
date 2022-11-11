CC = gcc

obj_dir = dist/target
exe_dir = dist
lib_dir = lib
src_dir = src

all: $(obj_dir) $(exe_dir)/test test

test: $(exe_dir)/test
	ln -s $(exe_dir)/test test


$(exe_dir)/test: $(src_dir)/test.c $(lib_dir)/liblibreria.a 
	gcc -o $(exe_dir)/test $(src_dir)/test.c \
		-iquote $(lib_dir) \
		-L $(lib_dir) -llibreria

debug: $(exe_dir)/debug
	ln -s $(exe_dir)/debug debug

$(exe_dir)/debug: $(obj_dir) $(src_dir)/test.c $(src_dir)/libreria.c
	gcc -g -o $(exe_dir)/debug $(src_dir)/test.c $(src_dir)/libreria.c

$(lib_dir)/liblibreria.a: $(obj_dir)/libreria.o
	ar -r $(lib_dir)/liblibreria.a $(obj_dir)/libreria.o

$(obj_dir)/libreria.o: $(src_dir)/libreria.c $(lib_dir)/libreria.h
	gcc -c $(src_dir)/libreria.c -o $(obj_dir)/libreria.o

$(obj_dir):
	mkdir -p $(obj_dir)
	
clean:
	rm -r $(exe_dir)

