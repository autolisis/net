#include <arpa/inet.h>
#include <arpa/tftp.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#define _BSD_SOURCE

#define BUFLEN 1024
#define endl printf("\n");

void except(char *error) {
	perror(error);
	exit(1);
}

void rawPrint(char *buf, size_t length) {
	endl for (int i = 0; i < length; i++) { printf("%02X ", buf[i]); }
}

int main(int argc, char *argv[]) {
	struct sockaddr_in sockServer, sockClient;
	int sockfd;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		except("ERROR OPENING SOCKET");
	bzero(&sockServer, sizeof(sockServer));
	sockServer.sin_family = AF_INET;
	sockServer.sin_addr.s_addr = htonl(INADDR_ANY);
	sockServer.sin_port = htons(69);
	if (argc == 2)
		sockServer.sin_port = htons(atoi(argv[1]));=
	int optval;
	optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));
	if ((bind(sockfd, (struct sockaddr *)&sockServer, sizeof(sockServer)) < 0))
		except("FAILED TO BIND, IS THE ADDRESS ALREADY IN USE?");

	char *buf = calloc(1, BUFLEN);
	/* printf("%lu", sizeof(packet)); */
	/*		fflush(stdout); */
	while (1) {
		bzero(buf, BUFLEN);
		size_t packetLength;
		if (packetLength > 0) {
		}
	}
	wait(NULL);
	return 0;
}
