CC := gcc

CFLAGS := -pedantic-errors -Wall -Werror -Wextra -std=c11

DIRS := src/

SRCS := $(shell find $(DIRS) -name *.c)

LFLAGS := -lpthread

BIN := example

FILES := logs1.txt logs2.txt

all: CFLAGS +=-O2
all: release

debug: CFLAGS +=-O0 -ggdb
debug: release

release:
	$(CC) $(CFLAGS) $(SRCS) -o $(BIN) $(LFLAGS) -DLCOLOR

clean:
	$(RM) $(BIN) $(FILES)
