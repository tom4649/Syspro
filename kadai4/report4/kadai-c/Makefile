#!/bin/sh

iperfc :iperfc.c
	gcc -Wall -O2 -o iperfc iperfc.c
iperfs :iperfs.c
	gcc -Wall -O2 -o iperfs iperfs.c
.PHONY: clean
clean:
	rm -f  iperfc iperfs 