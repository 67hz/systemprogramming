# pass EXEC as var to make
# EXEC = copy 
OBJECTS = 
BIN_DIR = ./bin
SYSPROG_DIR = ../lib
SRC_FILES = $(SYSPROG_DIR)/*.c 

DEBUG_LEVEL = -ggdb3
CXX = gcc
CC = $(CXX)

CFLAGS= -std=c99 $(DEBUG_LEVEL) -D_XOPEN_SOURCE=600
CPPFLAGS += -I$(SYSPROG_DIR) $(SRC_FILES)
LDFLAGS = -pthread


$(basename $(EXEC)): $(SRC_FILES)
	@echo this one : $@
	$(CC) $(CFLAGS) $(CPPFLAGS) $@.c $(LDFLAGS) -o $(BIN_DIR)/$@


clean:
	rm ./bin/*

phony: .clean
