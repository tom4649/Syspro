#!/bin/sh

safe_btree :main.c btree.h
	gcc -Wall -pthread -O2 -o safe_btree main.c btree.c
.PHONY: clean
clean:
	rm -f  safe_btree