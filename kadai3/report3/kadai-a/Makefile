#!/bin/sh

unsafe_btree :main.c btree.h
	gcc -Wall -pthread -O2 -o unsafe_btree main.c btree.c
.PHONY: clean
clean:
	rm -f  unsafe_btree