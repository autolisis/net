#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#define BUFLEN 1024
#define endl printf("\n");
struct sockaddr_in sockServer;

void except(char *error) {
	perror(error);
	exit(1);
}

#define RRQ 01	 /* read request */
#define WRQ 02	 /* write request */
#define DATA 03  /* data packet */
#define ACK 04	 /* acknowledgement */
#define ERROR 05 /* error code */

typedef struct packet {
	short opcode;
	union {
		unsigned short block;
		short code;
	};
	char *data;
	char *stuff;
} TFTPPACK;

TFTPPACK packet;

void rawPrint(char *buf, size_t length) {
	for (int i = 0; i < length; i++) {
		printf("%02X", buf[i]);
	}
}

void TFTPRead(char *filename) {
	bzero(&packet, sizeof(packet));
}

void TFTPWrite(char *filename) {
	if (access(filename, F_OK) == -1)
		except("FILE DOES NOT EXIST.");
		



	}
}

void parse(char *command) {
	char *token = strtok(command, " ");
	if (strcasecmp(token, "get") == 0) {
		token = strtok(NULL, " ");
		TFTPRead(token);
	}
	else if (strcasecmp(token, "put") == 0) {
		token = strtok(NULL, " ");
		TFTPWrite(token);
	}
}

int main(int argc, char *argv[]) {
	int s, i;
	socklen_t slen = sizeof(sockServer);
	char buf[BUFLEN];
	char message[BUFLEN];

	char SERVER[] = "127.0.0.1";
	short PORT = 69;
	/* if (argc < 2) */
	/* 	except("Please enter server IP."); */
	/* char *SERVER = 0; */
	/* SERVER = strdup(argv[1]); */
	/* short PORT = 69; */


	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		except("socket");
	}

	memset((char *)&sockServer, 0, sizeof(sockServer));
	sockServer.sin_family = AF_INET;
	sockServer.sin_port = htons(PORT);

	if (inet_aton(SERVER, &sockServer.sin_addr) == 0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}
	printf("TFTP SERVER:\nDoes not support spaces in filenames");

	using_history();
	read_history(".tftphist");

	while (1) {
		char *command = readline("TFTP > ");
		if (command) {
		add_history(command);
		write_history(".tftphist");
		}
		parse(command);

		if (sendto(s, message, strlen(message), 0, (struct sockaddr *)&sockServer,
							slen) == -1) {
			except("sendto()");
		}

		// receive a reply and print it
		// clear the buffer by filling null, it might have previously received data
		bzero(buf, BUFLEN);
		// try to receive some data, this is a blocking call
		if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *)&sockServer, &slen) ==
				-1) {
			except("recvfrom()");
		}

		puts(buf);
	}

	close(s);
	return 0;
}
