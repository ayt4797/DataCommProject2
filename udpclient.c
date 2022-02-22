/*
 * From https://gist.github.com/suyash/0f100b1518334fcf650bbefd54556df9
 *
 * Refactored by NHP
 */
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
/*
int main() {
	const char* server_name = "localhost";
	const int server_port = 8877;

	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;

	// creates binary representation of server name
	// and stores it as sin_addr
	// http://beej.us/guide/bgnet/output/html/multipage/inet_ntopman.html
	inet_pton(AF_INET, server_name, &server_address.sin_addr);

	// htons: port in network order format
	server_address.sin_port = htons(server_port);

	// open socket
	int sock;
	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("could not create socket\n");
		return 1;
	}

	// data that will be sent to the server
	const char* data_to_send = "Gangadhar Hi Shaktimaan hai";

	// send data
	int len =
	    sendto(sock, data_to_send, strlen(data_to_send), 0,
	           (struct sockaddr*)&server_address, sizeof(server_address));

	// received echoed data back
	char buffer[100];
	recvfrom(sock, buffer, len, 0, NULL, NULL);

	buffer[len] = '\0';
	printf("recieved: '%s'\n", buffer);

	// close the socket
	close(sock);
	return 0;
}
*/

#define BUF_SIZE    8192

int main(int argc, char *argv[]) {
  //const char* server_name = "localhost";  
  time_t timer;
  in_addr_t ip_addr;
  if(argc <2){
    printf("please give an ip addr & a timeout value");
    return -99;
  }
  else{
		ip_addr = inet_addr(argv[1]);
    timer = atof(argv[2]);

  }
  const int server_port = 8888;

  struct sockaddr_in server_address;
  
  char buffer[BUF_SIZE+1];
  char *data_to_send = buffer;
  
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;

  // creates binary representation of server name
  // and stores it as sin_addr
  // http://beej.us/guide/bgnet/output/html/multipage/inet_ntopman.html
  //inet_pton(AF_INET, server_name, &server_address.sin_addr);

  server_address.sin_addr.s_addr = htonl( INADDR_LOOPBACK );
  // htons: port in network order format
  server_address.sin_port = htons(server_port);

  // open socket
  int sock;
  if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
    printf("could not create socket\n");
    return 1;
  }
  time_t start,end,t;

  // Loop here until we get a SIGHUP or other interrupting signal
  for (;;) {
    printf("Prompt> ");
    memset( data_to_send, 0x00, BUF_SIZE + 1);
    // data that will be sent to the server
    fgets(data_to_send, BUF_SIZE, stdin);

    // remove trailing \n from the user input
    data_to_send[strlen(data_to_send) - 1] = 0x00;    
    start=clock();//predefined  function in c
    //after the user defined function does its work
    // send data
    int len =
        sendto(sock, data_to_send, strlen(data_to_send), 0,
               (struct sockaddr*)&server_address, sizeof(server_address));

    // received echoed data back
    //char buffer[100];
    // if(timeout)
   //do I need to do this in another proccess?
    end=clock();
    t=(end-start)/CLOCKS_PER_SEC;
    if(t>=timer){
        printf("FAILURE\n");
        continue;
    }
    recvfrom(sock, buffer, len, 0, NULL, NULL);
    printf("SUCCESSES\n");
    buffer[len] = '\0';
    printf("reply: '%s'\n", buffer);

  }
  
  // close the socket
  close(sock);
  return 0;
}
