   SRCS = $(wildcard *.c)
   OBJS = $(SRCS:%.c=%.o)
TARGETS = $(basename $(SRCS))
     CC = gcc
     LD = gcc
 CFLAGS = -Wall

.PHONY: all
all: $(OBJS) $(TARGETS)
%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<
%:%.o
	$(LD) -o $@ $<

.PHONY: clean
clean:
	rm -f $(TARGETS) $(OBJS)
