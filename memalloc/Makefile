# pass EXEC as var to make
# EXEC = copy 
OBJECTS = 
SYSPROG_DIR = ../lib
BIN_DIR = ./bin
BUILD_DIR = ./build
# SRC_FILES are convenience functions for TLPI
SRC_FILES = $(SYSPROG_DIR)/*.c 

CXX = gcc
CC = $(CXX)

CFLAGS= -std=c99 -D_XOPEN_SOURCE=600  -I$(SYSPROG_DIR)
# LDFLAGS = -L$(SYSPROG_DIR) 

DEBUG_LEVEL = -g

INDENT_FLAGS_KERNEL = -nbad -bap -nbc -bbo -hnl -br -brs -c33 -cd33 \
-ncdb -ce -ci4 -cli0 -d0 -di1 -nfc1 -i8 -ip0 -l80 -lp -npcs -nprs -npsl -sai \
-saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1

$(EXEC): $(SRC_FILES)
	$(CC) $(CFLAGS) $(SRC_FILES) $@.c -o $(BIN_DIR)/$@


indent:
	$(info indenting $(EXEC) for Kernel style C)
	indent $(INDENT_FLAGS_KERNEL) $(EXEC)

clear-indent-src:
	rm *.c~

clear-indent-headers:
	rm *.h~

clean:
	rm ./bin/*
