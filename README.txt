TCP COMPILE & RUN INSTRUCTRIONS:

Go to the tcp folder: 
	you can start the server w/ ./server_tcp <IP ADDRESS OF THE THIS COMPUTER, THAT WILL ALSO ACT AS THE SERVER>
	The client can be started with ./client_tcp <IP ADDRESS OF THE COMPUTER HOSTING THE SERVER> <TIMEOUT VALUE>
Note: 
	If you don't provided either an ip or a timeout it will error out
	If you hit enter w/out passing any data it will end the session as though u hit ctrl c.
	If the server doesn't respond to the connect w/in the timeout it will end, if the server doesn't respond w/ a reply after a message sent it will simply continue and let the user know a timeout occured.

Compilation instructions: provided in the folder is a makefile, simply type make into any linux box w/ make installed

UDP COMPILE & RUN INSTRUCTRIONS:

Go to the UDP folder: 
	you can start the server w/ ./server_UDP <IP ADDRESS OF THE THIS COMPUTER, THAT WILL ALSO ACT AS THE SERVER>
	The client can be started with ./client_UDP <IP ADDRESS OF THE COMPUTER HOSTING THE SERVER> <TIMEOUT VALUE>
Note: 
	the client will indefinitally for a server
	If the server doesn't respond w/ a reply after a message sent it will simply continue and let the user know a timeout occured.

Compilation instructions: provided in the folder is a makefile, simply type make into any linux box w/ make installed