/*
 * This file is generated using rpcgen.
 * However some adjustments are made to support taking 
 * and setting logger host and port arguments (line 22-23, 76-78)
 */

#include "box_call.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.h"

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

// they are declared in server.h file, initialize here
char logger_host[255] = {}; 
int logger_port = 0;

static void
blackbox_call_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union {
		inputs call_blackbox_1_arg;
	} argument;
	char *result;
	xdrproc_t _xdr_argument, _xdr_result;
	char *(*local)(char *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
		return;

	case call_blackbox:
		_xdr_argument = (xdrproc_t) xdr_inputs;
		_xdr_result = (xdrproc_t) xdr_wrapstring;
		local = (char *(*)(char *, struct svc_req *)) call_blackbox_1_svc;
		break;

	default:
		svcerr_noproc (transp);
		return;
	}
	memset ((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		svcerr_decode (transp);
		return;
	}
	result = (*local)((char *)&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, (xdrproc_t) _xdr_result, result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		exit (1);
	}
	return;
}

int main (int argc, char **argv)
{
	// get logger host and port from argv
    if (argc < 3)
	{
		printf("logger host and port arguments are required.");
		return -1;
	}

	// they are globally available, set their values
	if (strcmp(argv[1], "localhost") == 0)
		strcpy(logger_host, "127.0.0.1");
	else
    	strcpy(logger_host, argv[1]);
    char* port_c = argv[2];
    logger_port = atoi(port_c);

	register SVCXPRT *transp;

	pmap_unset (BLACKBOX_CALL, CALL_VERSION);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, BLACKBOX_CALL, CALL_VERSION, blackbox_call_1, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (BLACKBOX_CALL, CALL_VERSION, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, BLACKBOX_CALL, CALL_VERSION, blackbox_call_1, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (BLACKBOX_CALL, CALL_VERSION, tcp).");
		exit(1);
	}

	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}
