#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

// define max buffer size and tcp packet size
#define BUFFER_SIZE 1024
#define PACKET_SIZE 256

// define tcp queue size
#define QUEUE_SIZE 10

// the function to read message from socket (clSockt) and write to the logfile
void log_request(int clSockt, char* logfile)
{
    // initialize a buffer with packet size
    char msg[PACKET_SIZE];

    // read from socket a maximum of packet size bytes
    read(clSockt, msg, PACKET_SIZE);

    // if no content is read, return
    if (strlen(msg) == 0)
        return;
    
    // open the log file, write the message in it and close
	FILE *fp = fopen(logfile, "a");
	fprintf(fp, "%s", msg);
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
        int cl_sockt = accept(sockt, (struct sockaddr*)&client_addr, sizeof(client_addr));
        if (cl_sockt < 0) // if cannot accept, continue to try
            continue;
        // process the request in a function
        log_request(cl_sockt, logfile);
    }

    return 0;
}