#!/bin/sh

myexec :myexec.c
	gcc -Wall -O2 -pthread -o myexec myexec.c
.PHONY: clean
clean:
	rm -f  myexec