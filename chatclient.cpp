// Name: Louisa Katlubeck
// Program 1 CS 372
// The testing machine was flip 1
// Description: chatclient is the chat client that communicates with chatserve until one person enters
// \quit to end the chat session
// The client handle and port are specified as arguments on the command line
// In that case chatserve will reopen the connection until it receives a SIGINT
// Sources consulted and / or code borrowed from:
// https://beej.us/guide/bgnet/html/single/bgnet.html#twotypes, 
// http://www.cplusplus.com/forum/windows/3224/,
// https://www.thecrazyprogrammer.com/2017/06/socket-programming.html,
// https://stackoverflow.com/questions/1641182/how-can-i-catch-a-ctrl-c-event-c

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <iostream>
#include <stdio.h>
#include <signal.h>
#include <cstdlib>

using namespace std;


// global constant strings
char closedMessage[1024] = "The client has closed the connection";
char checkForClosure[1024] = "The server has closed the connection\0";
char checkForEnd[1024] = "The server has ended the connection\0";

// handle setup
char handle[11];
char extra[4] = "> ";

///////////////////////////////////////////////////////////////////////
// description: createSocket() creates a socket
// receives: a port and host
// returns: a socket
///////////////////////////////////////////////////////////////////////
int createSocket(int port, char* host) {
	char dataReceived[1024];

	// establish a connection
	memset(dataReceived, '0', sizeof(dataReceived));
	return socket(AF_INET, SOCK_STREAM, 0);
}


///////////////////////////////////////////////////////////////////////
// description: connectToServer() connects to the server
// receives: a socket, port, and host
// returns: 1 if the connection failed, or 0 if the connection succeeded
///////////////////////////////////////////////////////////////////////
int connectToServer(int CreateSocket, int port, char* host) {
	struct sockaddr_in ipOfServer;

	ipOfServer.sin_family = AF_INET;
	ipOfServer.sin_port = htons(port);
	ipOfServer.sin_addr.s_addr = inet_addr(host);

	// connect to the server
	if (connect(CreateSocket, (struct sockaddr *)&ipOfServer, sizeof(ipOfServer))<0)
	{
		printf("Connection failed due to port and ip problems");
		return 1;
	}
	else return 0;
}


///////////////////////////////////////////////////////////////////////
// description: sendall sends all bytes in the buffer
// receives: a socket, buffer, and length
// returns: -1 on failure to send, 0 on successful send
// source: this function was taken directly from Beej's Guide
///////////////////////////////////////////////////////////////////////

int sendall(int s, char *buf, int *len)
{
	int total = 0;        // how many bytes we've sent
	int bytesleft = *len; // how many we have left to send
	int n;

	while (total < *len) {
		n = send(s, buf + total, bytesleft, 0);
		fflush(stdout);
		if (n == -1) { break; }
		total += n;
		bytesleft -= n;
	}

	*len = total; 		// return number actually sent here

	return n == -1 ? -1 : 0; 	// return -1 on failure, 0 on success
}


///////////////////////////////////////////////////////////////////////
// description: sendMessage() sends the message from the client to the server, and also checks to see if the client entered
// '\quit' to close the connection
// receives: a socket and a bool saying whether we need to start a new connection on \quit
// returns: nothing
///////////////////////////////////////////////////////////////////////
void sendMessage(int CreateSocket, bool &newConnect) {
	bool check = true;
	char space[2];
	space[0] = ' ';
	bool keepGoing = true;
	int length;
	char buffer[1024];

	cout << handle << "> ";
	fflush(stdout);
	cin >> buffer;

	int c = cin.peek();

	// check to see if \quit entered
	if (strcmp(buffer, "\\quit") == 0) {
		printf("Client has closed the connection\n");
		fflush(stdout);
		send(CreateSocket, closedMessage, strlen(closedMessage), 0);
		check = false;
		newConnect = true;
	}

	// else check for an error in sending the different parts of the message
	else if ((send(CreateSocket, handle, strlen(handle), 0)) == -1) {
		printf("Failure Sending Message");
		fflush(stdout);
		close(CreateSocket);
		check = false;
	}

	else if ((send(CreateSocket, extra, strlen(extra), 0)) == -1) {
		printf("Failure Sending Message");
		fflush(stdout);
		close(CreateSocket);
		check = false;
	}

	else {
		// send the first word
		length = strlen(buffer);
		sendall(CreateSocket, buffer, &length);

		// while there are other words
		while (c < 10 || c > 10) {
			cin >> buffer;
			length = strlen(buffer);
			send(CreateSocket, space, strlen(space), 0);
			sendall(CreateSocket, buffer, &length);
			c = cin.peek();
		}
	}

	return;
}


///////////////////////////////////////////////////////////////////////
// description: receiveMessage() receives and prints the message from the server
// receives: the socket, and the address of two bools to see if they client needs to reconnect
// if the server sent \quit, or if the server has ended the connection
// returns: true if the client got the server's message and the connection is open without modification,
// or false if the server either closed the connection, there are errors, or if the client needs to restart the connection
///////////////////////////////////////////////////////////////////////
bool receiveMessage(int CreateSocket, bool &newConnect, bool &endProgram) {
	int n;
	bool gotMssg;
	char serverMessage[1024];

	n = recv(CreateSocket, serverMessage, sizeof(serverMessage), 0);
	if (n <= 0) {
		printf("Either Connection Closed or Error\n");
		fflush(stdout);
		gotMssg = false;
		newConnect = true;
		return gotMssg;
	}

	else {
		serverMessage[n] = '\0';
		bool check = strcmp(serverMessage, checkForClosure);
		bool theEnd = strcmp(serverMessage, checkForEnd);

		// check to see if the server has closed the connection
		if (!check) {
			gotMssg = false;
			cout << "There are " << gotMssg << " active connections to the server. The server has closed the connection.";
			fflush(stdout);
			cout << "\n";
			return gotMssg;
		}

		else if (!theEnd) {
			cout << "The server has ended the connection.\n";
			fflush(stdout);
			endProgram = true;
			gotMssg = false;
			return gotMssg;
		}

		else {
			// get the message from the server and print it
			printf("%s\n", serverMessage);
			fflush(stdout);
			gotMssg = true;
			return gotMssg;
		}
	}
}

///////////////////////////////////////////////////////////////////////
// main() part of the program
///////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	// variable setup
	int CreateSocket = 0, n = 0, r = 0, s = 0;
	int port = 0;
	char* host;
	bool gotMssg = true;
	bool newConnect = false;
	int connect = true;
	bool endProgram = false;
	char buffer[1024] = { 0 };

	bool go = true;
	char serverMessage[1024];

	while (!newConnect && !endProgram) {
		gotMssg = true;

		// get the handle
		cout << "Enter your handle: ";
		cin >> handle;

		// get the hostname and port number
		host = argv[1];
		port = atoi(argv[2]);

		// create a socket
		CreateSocket = createSocket(port, host);

		// error check the socket
		if ((CreateSocket)< 0)
		{
			printf("Socket not created \n");
			return 1;
		}

		connect = connectToServer(CreateSocket, port, host);

		while (gotMssg && !newConnect) {
			// send a message to the server
			sendMessage(CreateSocket, newConnect);
			if (!newConnect) gotMssg = receiveMessage(CreateSocket, newConnect, endProgram);
		}

		if (!endProgram) {
			close(CreateSocket);
			newConnect = false;
		}
	}

	// close the socket
	close(CreateSocket);

	// return from main
	return 0;
}


