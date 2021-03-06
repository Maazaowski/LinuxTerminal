#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

#define h_addr h_addr_list[0]

struct Processes {

    pid_t PID;
    int status;
    char Pname[1000];
    time_t start_time;
    time_t end_time;
    int elapsed_time;
};

struct Processes process_list[100];

int main(int argc, char *argv[])
	{
	//printf("asdonzvcxnadfn");
	int sock;
	struct sockaddr_in server;
	struct hostent *hp;
	char buf[1024];

	/* Create socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("opening stream socket");
		exit(1);
	}
	/* Connect socket using name specified by command line. */
	server.sin_family = AF_INET;
	hp = gethostbyname(argv[1]);
	if (hp == 0) {
		printf("%d", hp);
		fprintf(stderr, "%s: unknown host\n", argv[1]);
		exit(2);
	}
	bcopy(hp->h_addr, &server.sin_addr, hp->h_length);
	server.sin_port = htons(atoi(argv[2]));

	if (connect(sock,(struct sockaddr *) &server,sizeof(server)) < 0) {
		perror("connecting stream socket");
		exit(1);
	}
	while(1) {
        char cd[8] = "maaz:~$ ";
        int a = write(STDOUT_FILENO, cd, sizeof(cd));
        if (a == -1)
        {
            perror("write");
            exit(-1);
        }
        char command[100];
        int ci = read(STDIN_FILENO, command, sizeof(command));

        if (ci == -1)
        {
            perror("read");
            exit(-1);
        }

        command[ci-1] = '\0';
	  
	if (write(sock, command, sizeof(command)) < 0)
		perror("writing on stream socket");
	//getchar();
	  
	}
	close(sock);
}