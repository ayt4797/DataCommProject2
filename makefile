.PHONY: all
.SILENT: 

cc = gcc -Wall -std=c99

all: 
	$(cc) udpclient.c -o client
	$(cc) udpserver.c -o server