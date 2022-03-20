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
#define MAX_CLIENTS 4
struct cmd_s
{
	uint16_t type;
	uint16_t length;
	char buffer[BUF_SIZE];
};
void communicateWithClient(int accepted,struct sockaddr_in client_address,int* read_counter);

int main(int argc, char *argv[])
{
	// port to start the server on
	// difference btwn in_addr vs in_addr_t*
	int accepted = 0;
	socklen_t client_address_size = 0;
	int read_counter;
	in_addr_t ip_addr;
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

	printf("\nconnection w/ tcp server\n");
	// run indefinitely
	fd_set rfds;
	fd_set rfds_accepted;

	struct timeval timev;
	timev.tv_sec = 1;
	timev.tv_usec = 0;
	
	int acceptedFd[MAX_CLIENTS];
	memset(acceptedFd,0,MAX_CLIENTS*sizeof(int));
	
	while (true)
	{
		FD_ZERO(&rfds);
		
		FD_SET(sock, &rfds);
		accepted=0;
		int s = select(sock + 1, &rfds, NULL, NULL, &timev);
		if (s == -1)
		{
			printf("select failed");
		}
		else if(s!=0)
		{
			accepted = accept(sock, (struct sockaddr *)&client_address, &client_address_size);
		}

		for (int i=0;i<MAX_CLIENTS;i++){
			if(acceptedFd[i]!=0){
				FD_SET(acceptedFd[i],&rfds_accepted);
			}
			if(acceptedFd[i]==0&&accepted!=0){
				printf("SOCKETS OPEN");
				fflush(stdout);
				acceptedFd[i]=accepted;
				break;
			}
		}
		int s_accepted = select(1024, &rfds_accepted, NULL, NULL, &timev);
		if(s_accepted==-1){
			perror("select on accepted failed");
		}
		else if(s_accepted!=0){
			for (int i=0;i<MAX_CLIENTS;i++){
				if(acceptedFd[i]!=0 && FD_ISSET(acceptedFd[i],&rfds_accepted)){
					printf("HERE!");
					communicateWithClient(acceptedFd[i],client_address,&read_counter);
				}
			}
		}
		// if (accepted == 0)
		// 	continue;
		if (accepted == -1)
		{
			perror("couldn't accept socket ending");
			return -99;
		}

	}
	close(sock);
	return 0;
}
void communicateWithClient(int accepted,struct sockaddr_in client_address,int* read_counter)
{
	socklen_t client_address_len = sizeof(client_address);
	char hostname[128];
	char service_name[128];
	char buffer[BUF_SIZE];

	memset(buffer, 0, BUF_SIZE );
	printf("Calling Rcv\n");
	int len = recv(accepted, buffer, BUF_SIZE, 0);
	if(len==0){
		printf("Nothing sent");
		exit(0);
	}
	uint16_t tst;
	memset(&tst, 0, sizeof(tst));
	memcpy(&tst, buffer, sizeof(tst));
	
	printf("type : %hu ",tst);

	printf("BUFFER: %s\n", buffer);
	if (len == -1)
	{
		perror("error on reading");
		exit(1);
	}
	*read_counter += 1;
	printf("read counter: %d\n", *read_counter);
	struct addrinfo hints = {0}; // filters
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	struct addrinfo *result;
	char *client_ip = inet_ntoa(client_address.sin_addr);
	if (getaddrinfo(client_ip, NULL, &hints, &result) == -1)
	{
		perror("error getting host name\n");
		free(result);
		exit(1);
	}
	else
	{
		for (struct addrinfo *res = result; res != NULL; res = res->ai_next)
		{

			getnameinfo(res->ai_addr, res->ai_addrlen,
						hostname, sizeof(hostname),
						service_name, sizeof(service_name), 0);
		}
		freeaddrinfo(result);
	}

	// seperate header fr;om packet:
	struct cmd_s msg;
	memset(&msg.length, 0, sizeof(msg.length));

	memcpy(&msg.type, buffer, sizeof(msg.type));
	memcpy(&msg.length, buffer + sizeof(msg.type), sizeof(msg.length));
	msg.type = htons(msg.type);
	msg.length = htons(msg.length);
	memset(msg.buffer, 0, sizeof(msg.buffer));
	if (msg.type == (0x7eef))
		memcpy(msg.buffer, buffer + sizeof(msg.type) + sizeof(msg.length), sizeof(buffer) - ((sizeof(msg.type) + sizeof(msg.length))));
	else
	{
		msg.buffer[0] = '\0';
	}
	// inet_ntoa prints user friendly representation of the ip address
	printf("rcvd: '%s' from %s %u %d <%s>, service: %s type: %hu length: %hu \n",
		   msg.buffer,
		   inet_ntoa(client_address.sin_addr),
		   ntohs(client_address.sin_port),
		   client_address_len, hostname, service_name, msg.type, msg.length);

	if (msg.type == (0x7eef))
	{
		int len_send = sendto(accepted, msg.buffer, msg.length, 0, (struct sockaddr *)&client_address,
							  sizeof(client_address));

		if (len_send == -1)
		{
			perror("error sending back");
			exit(1);
		}
	}
	else
	{
		printf("\nDisconnecting\n");
		close(accepted);
		exit(0);
	}
	printf("end of first while loop");
	fflush(stdout);
	// close(accepted);
}
