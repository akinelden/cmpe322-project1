/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include <stdio.h>
#include "box_call.h"

// this function calls the server with given parameters and returns the result string
char *call_server(char *server, char *blackbox, int first, int second)
{
	// the rpc client
	CLIENT *client;
	// the input arguments stored in a struct
	inputs arguments = {blackbox, first, second};

#ifndef DEBUG
	client = clnt_create(server, BLACKBOX_CALL, CALL_VERSION, "udp");
	if (client == NULL)
		exit(-1);
#endif /* DEBUG */

	char **result = call_blackbox_1(&arguments, client);
	if (result == (char **)NULL)
		exit(-1);

#ifndef DEBUG
	clnt_destroy(client);
#endif /* DEBUG */
	return *result;
}

int main(int argc, char *argv[])
{

	if (argc < 4)
	{
		printf("blackbox, outputfile and host arguments are required.");
		return -1;
	}
	// path of blackbox
	char *blackbox = argv[1];
	// path of output file
	char *outputPath = argv[2];
	// address of server
	char *server = argv[3];

	// input integers
	int first, second;
	// read input from console
	scanf("%d", &first);
	scanf("%d", &second);

	// call server with the input parameters
	char* result = call_server(server, blackbox, first, second);

	// open the output file and write content to it
	FILE *fp = fopen(outputPath, "a");
	fprintf(fp, "%s", result);
	fclose(fp);

	return 0;
}
