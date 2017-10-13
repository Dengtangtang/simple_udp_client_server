#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>     /* defines STDIN_FILENO, system calls,etc */
#include <sys/types.h>  /* system data type definitions */
#include <sys/socket.h> /* socket specific definitions */
#include <netinet/in.h> /* INET constants and stuff */
#include <arpa/inet.h>  /* IP address conversion stuff */
#include <netdb.h>      /* gethostbyname */
#include <string.h>

#define MAXBUF 1024*1024

void echo(int sd) {
	char bufin[MAXBUF];
	struct sockaddr_in remote;
	socklen_t len;
	int n;
	int num = 1;
	char tmp[16];

	/* need to know how big address struct is, len must be set before the
	   call to recvfrom!!! 
	*/
	
	len = sizeof(remote);

	while (1) {
		/* clean */

		memset(tmp , 0, strlen(tmp));
		memset(bufin , 0, strlen(bufin));

		/* read a datagram from the socket */

		n = recvfrom(sd, bufin, MAXBUF, 0, (struct sockaddr*) &remote, &len);

		/* print out the address of the sender */

		printf("Got a datagram %s from %s port %d\n", bufin, inet_ntoa(remote.sin_addr), ntohs(remote.sin_port));

		if (n < 0) {
			perror("errors on receiving data!");
			// exit(1);
		}
		else {
			/* add number at the beginning of the received message 
			   and send it back
			*/

			sprintf(tmp, "%d", num);
			strcat(tmp, bufin);
			strcpy(bufin, tmp);
			int bufin_len = strlen(bufin);
			num++;

			sendto(sd, bufin, bufin_len, 0, (struct sockaddr*) &remote, len);
		}
	}
}

int main() {

	int ld;
	struct sockaddr_in skaddr;
	//socklen_t length;

	/* create a socket 
	   udp uses SOCK_DGRAM as type
	*/

	ld = socket(PF_INET, SOCK_DGRAM, 0);
	if (ld < 0) {
		printf("%s\n", "Problem creating socket!");
		exit(1);
	}

	/* set up server */

	skaddr.sin_family = AF_INET;
	skaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	skaddr.sin_port = htons(9897);

	/* bind skaddr to socket 
	   we could call this "assingning name to socket"
	   
	   further, we do not usually need to bind() on client side,
	   kernel will help us to choose ip and port number
	*/

	if (bind(ld, (struct sockaddr*) &skaddr, sizeof(skaddr)) < 0) {
		printf("%s\n", "Problem binding");
		exit(1);
	}

	/* find out what port we were assigned and print it out */

	// length = sizeof( skaddr );
	// if (getsockname(ld, (struct sockaddr *) &skaddr, &length)<0) {
	// 	printf("Error getsockname\n");
	// 	exit(1);
	// }

	/* port number's are network byte order, we have to convert to
	   host byte order before printing !
	*/
  
	// printf("The server UDP port number is %d\n",ntohs(skaddr.sin_port));


	/* echo */
	echo(ld);

	return 0;
}