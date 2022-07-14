#!/bin/sh

tcpechoclient :tcpechoclient.c
	gcc -Wall -O2 -o tcpechoclient tcpechoclient.c
tcpechoserver1 :tcpechoserver1.c
	gcc -Wall -O2 -pthread -o tcpechoserver1 tcpechoserver1.c
tcpechoserver2 :tcpechoserver2.c
	gcc -Wall -O2 -o tcpechoserver2 tcpechoserver2.c

.PHONY: clean
clean:
	rm -f  tcpechoserver1 tcpechoclient tcpechoserver2