#!/bin/sh

udpechoserver :udpechoserver.c
	gcc -Wall -O2 -o udpechoserver udpechoserver.c
udpechoclient :udpechoclient.c
	gcc -Wall -O2 -o udpechoclient udpechoclient.c
.PHONY: clean
clean:
	rm -f  udpechoserver udpechoclient 