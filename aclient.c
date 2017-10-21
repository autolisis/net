#define _GNU_SOURCE
#define endl printf("\n");
#define flush fflush(stdout);fflush(stdin);fflush(stderr);

#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <signal.h>

int sockfd;

void fdcloser(int dummy) {
	system("echo -n");
	shutdown(sockfd, SHUT_RDWR);
	close(sockfd);
	exit(0);
}
void eprint(char *msg) {
	fprintf(stderr, "%s",msg);
}
void except(char *msg) {
	perror(msg);
	exit(0);
}

int copydup(int dest, int src) {
	int copyfd = dup(src);
	close(src);
	if (dup2(dest, src) < 0)
		except(NULL);
	return copyfd;
}

void getFunc(char *filename, int newsockfd);

int main(int argc, char *argv[]) {
	struct sockaddr_in serv_addr;
	socklen_t slen = sizeof(serv_addr);
	signal(SIGINT, fdcloser);


	bzero(&serv_addr, sizeof(serv_addr));
	char buf[100];
	bzero(buf, 100);
	char *endString = strdup("$END$");
	/* if (argc > 3 || argc < 2) { */
	/* 	snprintf(buf, 100, "usage: %s <address> [port=4567]", argv[0]); */
	/* 	endl */
	/* 	except(buf); */
	/* } */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		except("Cannot create socket\n");
	int optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (const void *)&optval, sizeof(int));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	/* serv_addr.sin_addr.s_addr = inet_addr(argv[1]); */
	/* if(inet_aton(argv[1], &serv_addr.sin_addr) < 0) */
	/* 	except("Please enter VALID IP ADDRESS."); */
	serv_addr.sin_port = htons(4567);
	if (argc == 3)
		serv_addr.sin_port = htons(atoi(argv[2]));
	int connectStatus = connect(sockfd, &serv_addr, slen);
	if (connectStatus < 0)
		except("Failed to connect!");
	char message[100];
	/* savefd = copydup(sockfd, STDOUT_FILENO); */
	/* while (1) { */
	flush
	/* do { */
	int retval = getsockopt (sockfd, SOL_SOCKET, SO_ERROR, buf, &slen);
	if (retval != 0)
		return 0;
	bzero(buf, 100);
	bzero(message, 100);
	recv(sockfd, buf, 100, 0);
	printf("%s", buf);
	/* flush */
	gets(message);
	/* flush */
	send(sockfd, message, 100, 0);
	/* } */
	/* while (strcmp(message, "exit") != 0); */
	/* dup2(sockfd, 1); */
	dup2(STDOUT_FILENO, sockfd);
	/* recv */
	/* while (1) { */
	recv(sockfd, buf, 100, 0);
	/* 	printf("%s", buf); */
	/* } */
	/* sendfile(STDOUT_FILENO, sockfd, 0, 100); */
	/* flush */
	/* } */
	close(sockfd);
	return 0;
}

void getFunc(char *filename, int newsockfd) {
	printf("Sockfd %d New Read Request", newsockfd); endl
	filename = strtok(filename, "\n");
	int fd = open(basename(filename), O_RDONLY);
	if (fd < 0) {
		/* errorFunc(newsockfd, "File not found.\n"); */
		return;
	}
	/* If control reaches here, file exists and is readable */
	printf("Reading file %s\n", basename(filename));
	/* errorFunc(newsockfd, "Reading file \n"); */
	char *command = malloc(strlen(filename)+10);
	strcpy(command, "cat \"");
	strcat(command, basename(filename));
	strcat(command, "\"");
	/* size_t bytesSent; */
	/* Make a copy of STDOUT for the cat command */
	/* while ( (bytesSent = sendfile(newsockfd, fd, 0, 100)) > 0) */
	/* 	; */
	int stdoutcopy = copydup(newsockfd, STDOUT_FILENO);
	/* Execute the command */
	system(command);
	flush
	/* Restore STDIN */
	dup2(stdoutcopy, STDOUT_FILENO);
	close(stdoutcopy);
	return;
}
