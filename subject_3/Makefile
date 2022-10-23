CC = gcc
BASE_DIR = dist

# La operación patsubst se puede reemplazar por estas operaciones:
# $(foo:%.c=%.o) || $(foo:.c=.o)

source = $(wildcard *.c)
files_names = $(patsubst %.c,%,$(source))

objects = $(foreach file_name, $(files_names), $(BASE_DIR)/$(file_name)/%.o)
executable = $(foreach file_name, $(files_names), $(BASE_DIR)/$(file_name)/$(file_name))
dir_target = $(foreach dir_name, $(files_names), $(BASE_DIR)/$(dir_name)/)

all: $(dir_target) $(executable) 

$(executable): %: %.o
	$(CC) $< -o  $@
	
$(objects): %.c
	$(CC) -c $<	-o $@

$(dir_target):
	mkdir -p $@

clean:
	rm -rf dist/*
