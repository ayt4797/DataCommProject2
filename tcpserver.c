#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#define BUF_SIZE 1500
#define SERVER_PORT 8888



struct cmd_s{
	uint16_t type;
	uint16_t length;
	char buffer[BUF_SIZE];

};


int main(int argc, char *argv[])
{
	// port to start the server on
	// difference btwn in_addr vs in_addr_t*
	in_addr_t ip_addr;
	int read_counter = 0;
	if (argc > 1)
	{
		ip_addr = inet_addr(argv[1]);
	}
	else
	{
		ip_addr = htonl(SERVER_PORT); // double check this
	}
	if (argc == 1)
	{
		printf("\ngive an ip addr!\n");
	}
	// socket address used for the server
	struct sockaddr_in server_address;

	// socket address used to store client address
	struct sockaddr_in client_address;

	// Failure to seet the len initially causes first recvfrom() to
	// fail wrt getting the remote address
	socklen_t client_address_len = sizeof(client_address);

	char buffer[BUF_SIZE + 1];

	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET; // basically just saying were using the internet (should we use AF_UNIX)

	// htons: host to network short: transforms a value in host byte
	// ordering format to a short value in network byte ordering format
	server_address.sin_port = htons(SERVER_PORT);

	// htons: host to network long: same as htons but for long

	server_address.sin_addr.s_addr = ip_addr;

	// create a UDP socket, creation returns -1 on failure
	int sock;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("could not create socket\n");
		return 1;
	}

	int enable = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
	{
		printf("setsockopt(SO_REUSEADDR) failed");
		return 1;
	}

	// bind it to listen to the incoming connections on the created server
	// address, will return -1 on error
	if ((bind(sock, (struct sockaddr *)&server_address,
			  sizeof(server_address))) < 0)
	{
		printf("could not bind socket\n");
		return 1;
	}
	printf("binded on port : %d", SERVER_PORT);
	listen(sock, 5);
	printf("\nlistening\n");
	int accepted = 0;
	socklen_t client_address_size = 0;
	while (accepted == 0)
	{
		client_address_size = sizeof(client_address);
		accepted = accept(sock, (struct sockaddr *)&client_address, &client_address_size);
		// if(accepted<0){
		// 	printf("\nerror in accepting\n");
		// 	return -99;
		// }
	}
	char hostname[128];
	char service_name[128];

	printf("\nconnection w/ tcp server\n");
	// run indefinitely
	while (true)
	{
		memset(buffer, 0x00, BUF_SIZE + 1);
		printf("Calling Rcv\n");
		// read content into buffer from an incoming client
		int len = recvfrom(accepted, buffer, BUF_SIZE, 0,
						   (struct sockaddr *)&client_address,
						   &client_address_len);
		read_counter += 1;
		if (len < 1)
		{
			printf("Error on RX %d <%s>\n", errno, strerror(errno));
			continue;
		}
		else
		{
			struct addrinfo hints = {0}; // filters
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_DGRAM;
			struct addrinfo *result;
			char *client_ip = inet_ntoa(client_address.sin_addr);
			if (getaddrinfo(client_ip, NULL, &hints, &result) == -1)
			{
				printf("error getting host name\n");
				free(result);
				return -99;
			}
			else
			{
				for (struct addrinfo *res = result; res != NULL; res = res->ai_next)
				{
					getnameinfo(res->ai_addr, res->ai_addrlen,
								hostname, sizeof(hostname),
								service_name, sizeof(service_name), 0);
				}
			}
			
			freeaddrinfo(result);

			//seperate header from packet:
			struct cmd_s msg;
			msg.type=htons(0x7eef);
			msg.length=htons(len);
			memcpy(msg.buffer,buffer, len);
			int actLength=sizeof(msg.type)+sizeof(msg.length)+len;





			// inet_ntoa prints user friendly representation of the ip address
			printf("rcvd: '%s' from %s %u %d <%s>, service: %s\n",
				   buffer,
				   inet_ntoa(client_address.sin_addr),
				   ntohs(client_address.sin_port),
				   client_address_len, hostname, service_name);
			printf("read counter: %d\n", read_counter);
			// send same content back to the client ("echo")
			// I think this is where the ack is sent*
			int len_send = sendto(accepted, &msg, actLength, 0, (struct sockaddr *)&client_address,
								  sizeof(client_address));
			if (len_send == -1)
			{
				printf("error sending back");
				return -99;
			}
		}
	}
	close(sock);
	return 0;
}
