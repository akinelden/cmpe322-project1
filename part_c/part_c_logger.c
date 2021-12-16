// The project aims to execute four programs and communicate between them.
// The server and blackbox programs communicate through pipes, the server 
// and client applications communicate with RPC protocol and the server
// and logger programs communicate through TCP connection.
//
// The logger program takes two arguments from terminal, log file path and port.
// It creates a socket and binds the socket to given port. Then it starts to listen
// the requests. When a message arrives through socket, it reads the message, 
// writes its content to log file and sends back an "ok" message to inform the sender
// that message has arrived. Then continues to listen incoming requests.
//
// @author: Mehmet Akın Elden

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

// define max buffer size and tcp packet size
#define BUFFER_SIZE 1024

// define tcp queue size
#define QUEUE_SIZE 10

// the function to read message from socket (clSockt) and write to the logfile
void log_request(int clSockt, char* logfile)
{
    // initialize a buffer with packet size
    char msg[BUFFER_SIZE] = {};

    // read from socket a maximum of packet size bytes
    read(clSockt, msg, BUFFER_SIZE);

    // return an ok message to client socket so that it know transaction over
    write(clSockt, "ok", sizeof("ok"));

    // if no content is read, return
    if (strlen(msg) == 0)
        return;
    
    // open the log file, write the message in it and close
	FILE *fp = fopen(logfile, "a");
	fprintf(fp, "%s\n", msg);
	fclose(fp);
}

int main(int argc, char *argv[])
{

	if (argc < 3)
	{
		printf("log file and port arguments are required.");
		return -1;
	}
	// path of log file
	char *logfile = argv[1];
	// port address for tcp
	char *port_c = argv[2];
    int port = atoi(port_c);

    // create a stream socket with IPv4 domain using default protocol (TCP)
    int sockt = socket(AF_INET, SOCK_STREAM, 0);
    if (sockt < 0)
        return -1;

    // create server address
    struct sockaddr_in server_addr = {
        .sin_family = AF_INET, // IPv4
        .sin_addr = INADDR_ANY, // 0.0.0.0
        .sin_port = port
    };

    // bind socket to server_addr, if error occurs, exit
    if (bind(sockt, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
        return -1;
    
    // now listen the socket
    if (listen(sockt, QUEUE_SIZE) < 0)
        return -1;

    // define client socket address
    struct sockaddr_in client_addr;

    // now accept the connections in a loop
    while(1)
    {
        int addr_size = sizeof(client_addr);
        int cl_sockt = accept(sockt, (struct sockaddr*)&client_addr, &addr_size);
        if (cl_sockt < 0) // if cannot accept, continue to try
            continue;
        // process the request in a function
        log_request(cl_sockt, logfile);
    }

    return 0;
}