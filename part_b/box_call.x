/*
 * This file is used to generate rpc client and server codes
 * using rpcgen. There is one input struct and one program call.
 */

/* 
 * The input is struct, it includes a string for blackbox path,
 * and two integers for the inputs to blackbox
 */
struct inputs{
    string blackbox<1024>;
	int first;
	int second;
};

/* 
 * The program call takes inputs struct and returns a string
 * which is the result of the blackbox call.
 */
program BLACKBOX_CALL{
	version CALL_VERSION{
		/* Takes a numbers structure and gives the integer result. */
		string call_blackbox(inputs)=1;
	}=1;
}=0x13467955;