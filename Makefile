# Makefile templete is coming from ths websit: 
# https://www.cs.colby.edu/maxwell/courses/tutorials/CC=gcc
CFLAGS=-Wall -Wextra -g -pthread

MAIN=pro

# Source files - list all your .c files here
SRCS=main.c hunter.c ghost.c room.c utils.c evidence.c house.c logger.c

OBJS=$(SRCS:.c=.o)

.PHONY: clean all

all: $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(MAIN)
