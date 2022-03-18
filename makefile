.PHONY: all
.SILENT: 

cc = gcc -Wall 

all: 
	$(cc) udpclient.c -g -o client_udp
	$(cc) udpserver.c -g -o server_udp
	$(cc) tcpserver.c -g -o server_tcp
	$(cc) tcpclient.c -g -o client_tcp
	scp server_tcp ayt4797@eng-2500-10.main.ad.rit.edu:/home/ayt4797/datacommProject2
