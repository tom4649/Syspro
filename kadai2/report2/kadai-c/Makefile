#!/bin/sh

measure1 :measure1.c
	gcc -Wall -O2 -o measure1 measure1.c
measure2 :measure2.c
	gcc -Wall -O2 -o measure2 measure2.c
.PHONY: clean
clean:
	rm -f  measure1 measure2