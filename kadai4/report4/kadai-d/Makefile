#!/bin/sh

iperfc_double :iperfc_double.c
	gcc -Wall -O2 -pthread -o iperfc_double iperfc_double.c
iperfs_double :iperfs_double.c
	gcc -Wall -O2 -pthread -o iperfs_double iperfs_double.c
.PHONY: clean
clean:
	rm -f  iperfc_double iperfs_double 