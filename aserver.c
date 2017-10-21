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

void eprint(char *msg) {
	fprintf(stderr, "%s",msg);
}
void except(char *msg) {
	perror(msg);
	exit(0);
}
/* parses the buffer buf and functions on the fd newsockfd */
int parse(char *buf, size_t len, int newsockfd);

/* helper function to output an error string to the fd newsockfd */
void errorFunc(int newsockfd, char *error);

/* Lists the server root directory to the socket */
void listFunc(int newsockfd);

/* Writes a file given by the client from the socket */
void putFunc(char *buf, int newsockfd);

/* Reads a file and prints into the socket */
void getFunc(char *buf, int newsockfd);

int main(int argc, char *argv[]) {
	char *path = malloc(100);
	path = strdup(getenv("HOME"));
	strcat(path, "/srv");
	chdir(path);
	int sockfd;
	struct sockaddr_in cli_addr, serv_addr;
	socklen_t len = sizeof(cli_addr);

	char buf[100];
	bzero(buf, 100);
	if (argc > 2) {
		snprintf(buf, 100, "usage: %s [port=4567]", argv[0]);
		endl
		except(buf);
	}
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		except("Cannot create socket\n");
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(4567);
	if (argc == 2)
		serv_addr.sin_port = htons(atoi(argv[1]));

	int optval;
	optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));
	setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (const void *)&optval, sizeof(int));
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		except("Unable to bind local address\n");
	listen(sockfd, SOMAXCONN);
	while (1) {
		int newsockfd;
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &len);
		if (newsockfd < 0)
			except("Accept error\n");
		pid_t pid = fork();
		if (pid > 0) {
			printf("Sockfd %d assigned for new connection from %s:%d",
					newsockfd, inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
			endl
		}
		else if (pid == 0) {
			close(sockfd);
			char *prompt = strdup("Enter FTP command: ");
			send(newsockfd, prompt, strlen(prompt), 0);
			size_t len = recv(newsockfd, buf, 100, 0);
			if (parse(buf, len, newsockfd) == 0)
				break;
			/* send(newsockfd, prompt, strlen(prompt), 0); */
			len = recv(newsockfd, buf, 100, 0);
		}
		flush
		endl
		printf("Connection closed.");endl
		exit(0);
		close(newsockfd);
	}
	return 0;
}

int parse(char *buf, size_t len, int newsockfd) {
	char *tok;
	tok = strtok(buf, " \n");
	if (strcasecmp(tok, "exit") == 0)
		return 0;
	if (strcasecmp(tok, "ls") == 0) {
		listFunc(newsockfd);
		return 1;
	}
	else if (strcasecmp(tok, "get") == 0) {
		getFunc(buf+4, newsockfd);
		return 1;
	}
	else if (strcasecmp(tok, "put") == 0) {
		putFunc(buf+4, newsockfd);
		return 1;
	}
	else {
		errorFunc(newsockfd, "Invalid command.\nCommands are of the form get <filename>, put <filename>, ls, exit.\n");
		return 1;
	}
}

/* Helper function to redirect all of src fd to dest fd, while returning a copyfd to restore src */
int copydup(int dest, int src) {
	int copyfd = dup(src);
	close(src);
	if (dup2(dest, src) < 0)
		except(NULL);
	return copyfd;
}

void listFunc(int newsockfd) {
	printf("Sockfd %d New List Request", newsockfd); endl
	int stdoutCopy = copydup(newsockfd, STDOUT_FILENO);
	system("ls");
	if (dup2(stdoutCopy, STDOUT_FILENO) < 0)
		except(NULL);
	close(stdoutCopy);
	return;
}

void getFunc(char *filename, int newsockfd) {
	printf("Sockfd %d New Read Request", newsockfd); endl
	filename = strtok(filename, "\n");
	int fd = open(basename(filename), O_RDONLY);
	if (fd < 0) {
		errorFunc(newsockfd, "File not found.\n");
		return;
	}
	/* If control reaches here, file exists and is readable */
	printf("Reading file %s\n", basename(filename));
	errorFunc(newsockfd, "Reading file \n");
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

void putFunc(char *filename, int newsockfd) {
	filename = strtok(filename, "\n");
	int fd = open(basename(filename), O_CREAT | O_WRONLY);
	if (fd < 0) {
		errorFunc(newsockfd, "File not found.\n");
		return;
	}
	close(fd);
	/* If control reaches here, file is writeable */
	printf("Writing file %s\n\n", basename(filename));
	errorFunc(newsockfd, "Writing file\nEnter $END$ to stop writing\n");

	char *command = malloc(strlen(filename)+10);
	strcpy(command, "tee \"");
	strcat(command, basename(filename));
	strcat(command, "\"");

	int stdincopy = copydup(newsockfd, STDIN_FILENO);
	/* Execute the command */
	system(command);
	/* Restore STDIN */
	dup2(stdincopy, STDIN_FILENO);
	char *buf = calloc(100, 1);
	/* while ((len = recv(newsockfd, buf, 100, 0)) > 0) { */
	/* 	errorFunc(newsockfd, ">"); */
	/* } */
	close(stdincopy);
	/* close(fd); */
	return;
}

void errorFunc(int newsockfd, char *error) {
	send(newsockfd, error, strlen(error), 0);
}
