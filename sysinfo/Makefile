# pass EXEC as var to make
# EXEC = copy 
OBJECTS = 
SYSPROG_DIR = ../lib
USERS_DIR = ../users
BIN_DIR = ./bin
SRC_FILES = $(SYSPROG_DIR)/*.c $(USERS_DIR)/ugid_functions.c


CXX = gcc
CC = $(CXX)
DEBUG_LEVEL = -ggdb3

CFLAGS= $(DEBUG_LEVEL) -O0 -std=c99 -D_XOPEN_SOURCE=600  -I$(SYSPROG_DIR) -I../users
# LDFLAGS = -L$(SYSPROG_DIR) 



INDENT_FLAGS_KERNEL = -nbad -bap -nbc -bbo -hnl -br -brs -c33 -cd33 \
-ncdb -ce -ci4 -cli0 -d0 -di1 -nfc1 -i8 -ip0 -l80 -lp -npcs -nprs -npsl -sai \
-saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1

$(basename $(EXEC)): $(SRC_FILES)
	$(CC) $(CFLAGS) $(SRC_FILES) $@.c -lcrypt -o $(BIN_DIR)/$@ 


clean:
	rm -rf *.o

phony: .clean
