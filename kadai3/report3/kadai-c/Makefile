#!/bin/sh

bb :main.c bb.h
	gcc -Wall -pthread -O2 -o bb main.c bb.c
.PHONY: clean
clean:
	rm -f  bb