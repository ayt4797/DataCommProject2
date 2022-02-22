#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define BUF_SIZE    8192
#define	SERVER_PORT 8888


int main(int argc, char *argv[]) {
	// port to start the server on 
	//difference btwn in_addr vs in_addr_t*
	in_addr_t ip_addr;
	if(argc>1){ 
		ip_addr = inet_addr(argv[1]);
	}
	else{
		ip_addr= htonl(SERVER_PORT); //double check this
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
	server_address.sin_family = AF_INET; //basically just saying were using the internet (should we use AF_UNIX)

	// htons: host to network short: transforms a value in host byte
	// ordering format to a short value in network byte ordering format
	server_address.sin_port = htons(SERVER_PORT);

	// htons: host to network long: same as htons but for long
	
	server_address.sin_addr.s_addr = ip_addr;

	// create a UDP socket, creation returns -1 on failure
	int sock;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("could not create socket\n");
		return 1;
	}
	
	int enable = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
    printf("setsockopt(SO_REUSEADDR) failed");
    return 1;
  }


	// bind it to listen to the incoming connections on the created server
	// address, will return -1 on error
	if ((bind(sock, (struct sockaddr *)&server_address,
	          sizeof(server_address))) < 0) {
		printf("could not bind socket\n");
		return 1;
	}
  
	// run indefinitely
	while (true) 
  {
    memset( buffer, 0x00, BUF_SIZE+1 );
    printf("Calling Rcv\n");
		// read content into buffer from an incoming client
		int len = recvfrom(sock, buffer, BUF_SIZE, 0,
		                   (struct sockaddr *)&client_address,
		                   &client_address_len);

    if ( len < 1 ) 
    {
      printf("Error on RX %d <%s>\n", errno, strerror(errno) );
      continue;
    }
    else
    {
      // inet_ntoa prints user friendly representation of the ip address
      printf( "rcvd: '%s' from %s %u %d\n",
              buffer,
              inet_ntoa( client_address.sin_addr ),
              ntohs(client_address.sin_port),
              client_address_len );

      // send same content back to the client ("echo") 
	  //I think this is where the ack is sent*
      sendto(sock, buffer, len, 0, (struct sockaddr *)&client_address,
            sizeof(client_address));
    }
	}

	return 0;
}