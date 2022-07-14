#!bin/sh

CC = gcc
CFLAGS = -Wall -O2
LDFLAGES = -pthread -lm
TARGET = ish
SRCS := $(wildcard *.c) $(wildcard **/*.c) #wildcard
OBJS = $(SRCS:.c=.o) 
RM = rm -f
$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
clean:
	$(RM) $(TARGET) $(DEPS) $(OBJS)
