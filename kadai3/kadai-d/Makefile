#!/bin/sh

my_thread :my_thread.c
	gcc -Wall -O2 -o my_thread my_thread.c
test :test.c
	gcc -Wall  -O2 -o test test.c
.PHONY: clean
clean:
	rm -f  test my_thread