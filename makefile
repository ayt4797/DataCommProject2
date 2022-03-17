.PHONY: all
.SILENT: 

cc = gcc -Wall 

all: 
	$(cc) udpclient.c -g -o client_udp
	$(cc) udpserver.c -g -o server_udp
	$(cc) tcpserver.c -g -o server_tcp
	$(cc) tcpclient.c -g -o client_tcp
	scp client_udp ayt4797@eng-2500-10.main.ad.rit.edu:/home/ayt4797/datacommProject2
	scp server_udp ayt4797@eng-2500-11.main.ad.rit.edu:/home/ayt4797/datacommProject2
