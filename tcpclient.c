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
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/select.h>
#include <signal.h>

#define BUF_SIZE 1500
int sock = 0;
struct sockaddr_in server_address;
int p[2];
int con = -1;
struct cmd_s
{
	uint16_t type;
	uint16_t length;
	char buffer[BUF_SIZE];
};

int sendMessage(int type, char *buffer)
{
	struct cmd_s msg;
	msg.type = htons(type);
	memset(msg.buffer, 0, sizeof(msg.buffer));
	msg.length = htons(strlen(buffer));
	memcpy(msg.buffer, buffer, strlen(buffer));
	int actLength = sizeof(msg.type) + sizeof(msg.length) + strlen(buffer);
	int len =
		sendto(sock, &msg, actLength, 0,
			   (struct sockaddr *)&server_address, sizeof(server_address));
	return len;
}
void exitclient()
{
	sendMessage(0x5aa5, "\0");
	exit(0);
}
void exitHandler(int signal)
{
	if (con == -1)
	{ // not even connected
		exit(0);
	}
	char *buf = "Exit";

	if (write(p[1], buf, 5) == -1)
	{
		printf("write error");
	}
}

int main(int argc, char *argv[])
{

	if (pipe(p) < 0)
	{
		printf("piping failed");
		exit(0);
	}

	const int server_port = 8888;
	// make a pipe to connect to the signal handler
	//  const char* server_name = "localhost";
	signal(SIGINT, exitHandler);
	fd_set rfds;

	long double timer;
	in_addr_t ip_addr;
	char *data_to_send;
	char buffer[BUF_SIZE + 1];
	data_to_send = buffer;
	if (argc <= 2)
	{
		printf("please give an ip addr & a timeout value");
		return -99;
	}
	else
	{
		ip_addr = inet_addr(argv[1]);
		timer = atof(argv[2]);
	}

	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;

	server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	// htons: port in network order format
	server_address.sin_port = htons(server_port);
	server_address.sin_addr.s_addr = ip_addr;

	// open socket
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("could not create socket\n");
		return 1;
	}
	struct timeval timev;
	timev.tv_sec = timer;
	timev.tv_usec = 0;

	con = connect(sock, (struct sockaddr *)&server_address, sizeof(server_address));
	if (con != 0)
	{
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");
	// this doesn't work for some reason
	//  FD_ZERO(&rfds);
	//  FD_SET(sock, &rfds);
	//  FD_SET(p[0], &rfds);

	// Loop here until we get a SIGHUP or other interrupting signal
	for (;;)
	{
		//****if the usr wants to exit prematurally using ctrl c it requires the user to press enter to get into the select b/c it's stalled at fgets until it can get to the select loop, as all the ctrl c does is write to a pipe that controls exit
		printf("Prompt> ");
		memset(data_to_send, 0x00, BUF_SIZE + 1);

		fgets(data_to_send, BUF_SIZE, stdin);
		if(data_to_send[0]=='\n'){
			printf("exit client");
			exitclient();
		}

		data_to_send[strlen(data_to_send) - 1] = 0x00;
		// after the user defined function does its work
		//  send data
		int len = sendMessage(0x7eef, data_to_send);

		if (len == 0)
		{
			printf("no data to send exiting"); // need to document
			return 0;
		}
		else if (len < 0)
		{
			printf("send error error code: %d ", errno);
			return -99;
		}
		while (len < strlen(data_to_send))
		{
			printf("error sending packet sending additional packets");
			sendMessage(0x7eeef, buffer + len);
		}


		// setting the read side of the pipe
		// so that out of the select function so we can check if the fd_set has the pipe or the socket

		FD_ZERO(&rfds);
		FD_SET(sock, &rfds);
		FD_SET(p[0], &rfds);

		int s = select(1024, &rfds, NULL, NULL, &timev);
		if (s == -1)
		{
			perror("select failed!");
			continue;
		}
		else if (s == 0)
		{
			printf("TIMEOUT\n");
		}
		else
		{
				if (FD_ISSET(sock, &rfds))
				{
					ssize_t r = recvfrom(sock, buffer, BUF_SIZE, 0, NULL, NULL);
					if (strlen(data_to_send) == 0)
					{
						continue;
					}
					if (r == 0)
					{
						printf("server ended");
						close(sock);
						exit(0);
					}
					else if (r == -1)
					{
						printf("error in recvfrom");
						continue;
					}
					printf("SUCCESSES\n");
					buffer[r] = '\0';
					printf("reply: '%s'\n", buffer);
				}
				else
				{
					printf("here");
					char signalBuf[5];
					read(p[0], &signalBuf, 5);
					printf("signal buf: %s", signalBuf);
					if (strcmp(signalBuf, "Exit") == 0)
					{
						exitclient();
					}
				}
			}
	}

	// close the socket
	close(sock);
	return 0;
}
/*small message

000102030405060708091011121314151617181920212223242526272829303132333435363738394041424344454647484950515253545556575859606162636465666768697071727374757677787980818283848586878889909192939495969798990001020304050607080910111213141516171819202122232425262728293031323334353637383940414243444546474849505152535455565758596061626364656667686970717273747576777879808182838485868788899091929394959697989900010203040506070809101112131415161718192021222324252627282930313233343536373839404142434445464748495051525354555657585960616263646566676869707172737475767778798081828384858687888990919293949596979899
*/

/*large message

00010203040506070809101112131415161718192021222324252627282930313233343536373839404142434445464748495051525354555657585960616263646566676869707172737475767778798081828384858687888990919293949596979899000102030405060708091011121314151617181920212223242526272829303132333435363738394041424344454647484950515253545556575859606162636465666768697071727374757677787980818283848586878889909192939495969798990001020304050607080910111213141516171819202122232425262728293031323334353637383940414243444546474849505152535455565758596061626364656667686970717273747576777879808182838485868788899091929394959697989900010203040506070809101112131415161718192021222324252627282930313233343536373839404142434445464748495051525354555657585960616263646566676869707172737475767778798081828384858687888990919293949596979899000102030405060708091011121314151617181920212223242526272829303132333435363738394041424344454647484950515253545556575859606162636465666768697071727374757677787980818283848586878889909192939495969798990001020304050607080910111213141516171819202122232425262728293031323334353637383940414243444546474849505152535455565758596061626364656667686970717273747576777879808182838485868788899091929394959697989900010203040506070809101112131415161718192021222324252627282930313233343536373839404142434445464748495051525354555657585960616263646566676869707172737475767778798081828384858687888990919293949596979899000102030405060708091011121314151617181920212223242526272829303132333435363738394041424344454647484950515253545556575859606162636465666768697071727374757677787980818283848586878889909192939495969798990001020304050607080910111213141516171819202122232425262728293031323334353637383940414243444546474849505152535455565758596061626364656667686970717273747576777879808182838485868788899091929394959697989900010203040506070809101112131415161718192021222324252627282930313233343536373839404142434445464748495051525354555657585960616263646566676869707172737475767778798081828384858687888990919293949596979899000102030405060708091011121314151617181920212223242526272829303132333435363738394041424344454647484950515253545556575859606162636465666768697071727374757677787980818283848586878889909192939495969798990001020304050607080910111213141516171819202122232425262728293031323334353637383940414243444546474849505152535455565758596061626364656667686970717273747576777879808182838485868788899091929394959697989900010203040506070809101112131415161718192021222324252627282930313233343536373839404142434445464748495051525354555657585960616263646566676869707172737475767778798081828384858687888990919293949596979899000102030405060708091011121314151617181920212223242526272829303132333435363738394041424344454647484950515253545556575859606162636465666768697071727374757677787980818283848586878889909192939495969798990001020304050607080910111213141516171819202122232425262728293031323334353637383940414243444546474849505152535455565758596061626364656667686970717273747576777879808182838485868788899091929394959697989900010203040506070809101112131415161718192021222324252627282930313233343536373839404142434445464748495051525354555657585960616263646566676869707172737475767778798081828384858687888990919293949596979899000102030405060708091011121314151617181920212223242526272829303132333435363738394041424344454647484950515253545556575859606162636465666768697071727374757677787980818283848586878889909192939495969798990001020304050607080910111213141516171819202122232425262728293031323334353637383940414243444546474849505152535455565758596061626364656667686970717273747576777879808182838485868788899091929394959697989900010203040506070809101112131415161718192021222324252627282930313233343536373839404142434445464748495051525354555657585960616263646566676869707172737475767778798081828384858687888990919293949596979899000102030405060708091011121314151617181920212223242526272829303132333435363738394041424344454647484950515253545556575859606162636465666768697071727374757677787980818283848586878889909192939495969798990001020304050607080910111213141516171819202122232425262728293031323334353637383940414243444546474849505152535455565758596061626364656667686970717273747576777879808182838485868788899091929394959697989900010203040506070809101112131415161718192021222324252627282930313233343536373839404142434445464748495051525354555657585960616263646566676869707172737475767778798081828384858687888990919293949596979899000102030405060708091011121314151617181920212223242526272829303132333435363738394041424344454647484950515253545556575859606162636465666768697071727374757677787980818283848586878889909192939495969798990001020304050607080910111213141516171819202122232425262728293031323334353637383940414243444546474849505152535455565758596061626364656667686970717273747576777879808182838485868788899091929394959697989900010203040506070809101112131415161718192021222324252627282930313233343536373839404142434445464748495051525354555657585960616263646566676869707172737475767778798081828384858687888990919293949596979899
*/
/*

I need to make to pipe the signal to the main client, and have that pipe tell the main client to shut down
have the main while loop server to shut down via the signal handler,
move the cmdstruct into the client and have it use that message to commuicate

*/
