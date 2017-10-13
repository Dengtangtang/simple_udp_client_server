#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     /* defines STDIN_FILENO, system calls,etc */
#include <sys/types.h>  /* system data type definitions */
#include <sys/socket.h> /* socket specific definitions */
#include <netinet/in.h> /* INET constants and stuff */
#include <arpa/inet.h>  /* IP address conversion stuff */
#include <netdb.h>      /* gethostbyname */
#include <string.h>

#define MAXBUF 10*1024
#define TRAN_MSG "Hello"

int main() {
	int sk;
	struct sockaddr_in server;
	struct hostent *hp;
	char bufout[MAXBUF], bufin[MAXBUF];
	int bufout_len;

	/* create a socket 
	   udp uses SOCK_DGRAM as type
	*/

	sk = socket(PF_INET, SOCK_DGRAM, 0);
	if (sk < 0) {
		printf("Problem on creating sockets!\n");
		exit(1);
	}

	/* set server address family 
	   ip protocal uses AF_INET
	*/

	server.sin_family = AF_INET;

	/* get host information */ 

	hp = gethostbyname("localhost");

	/* copy ip addr into sockaddr_in
	   it is already in network bytes order
	*/

	memcpy(&(server.sin_addr.s_addr), hp->h_addr, hp->h_length); // h_addr is defined to be h_addr_list[0] (subscript returns reference)

	/* give port number to sockaddr_in
	   note that we shall use htons() to ensure portability on different systems
	   htons() transfers host bytes order into network bytes order
	*/

	server.sin_port = htons(9897);

	/* based on given requirment, client sends msg with number from 0 to 50 to the echo server */

	for (int i = 1; i <= 50; ++i) {

		/* clear */

		memset(bufout , 0, strlen(bufout));

		/* copy message to buffer */
		strcpy(bufout, TRAN_MSG);

		bufout_len = strlen(bufout);

		/* send data to echo server */

		int n_sent = sendto(sk, bufout, bufout_len, 0, (struct sockaddr*) &server, sizeof(server));

		if (n_sent < 0) {
			perror("errors on sending data!");
			exit(1);
		}

		if (n_sent != bufout_len)
			printf("Sendto sent %d bytes out of %d\n", n_sent, bufout_len);

		/* wait for a reply (from anyone) */

		int n_recv = recvfrom(sk, bufin, MAXBUF, 0, NULL, NULL);
		if (n_recv < 0) {
			perror("errors on receiving data!");
			exit(1);
		}

		/* send what we got back to stdout */

		bufin[n_recv]='\0';
		if (write(STDOUT_FILENO, bufin, n_recv + 1) < 0) {
			perror("Problem writing to stdout");
			exit(1);
		}
		printf("\n");

	}

	return 0;
}