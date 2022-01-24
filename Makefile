CC := gcc

CFLAGS := -pedantic-errors -Wall -Werror -Wextra -std=c11

DIRS := src/

SRCS := $(shell find $(CLIENTSRCDIRS) -name *.c)

LFLAGS := -lpthread

BIN := example

FILES := logs.txt

all: CFLAGS +=-O2
all: release

debug: CFLAGS +=-O0 -ggdb
debug: release

release:
	$(CC) $(CFLAGS) $(SRCS) -o $(BIN) $(LFLAGS)

clean:
	$(RM) $(BIN) $(FILES)
