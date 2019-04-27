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
#include <sys/select.h>

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
		//printf("%d", hp);
		fprintf(stderr, "%s: unknown host\n", argv[1]);
		exit(2);
	}
	bcopy(hp->h_addr, &server.sin_addr, hp->h_length);
	server.sin_port = htons(atoi(argv[2]));

	if (connect(sock,(struct sockaddr *) &server,sizeof(server)) < 0) {
		perror("connecting stream socket");
		exit(1);
	}

	fd_set readfds;
	struct timeval timeout;
	char command[100];

	while(1) {
		

		timeout.tv_sec = 1;
		timeout.tv_usec = 5;
		

        char cd[8] = "maaz:~$ ";
		char output[1000];
        int a = write(STDOUT_FILENO, cd, sizeof(cd));
        if (a == -1)
        {
            perror("write");
            exit(-1);
        }
        
        int ci = read(STDIN_FILENO, command, sizeof(command));
	command[ci-1] = '\0';

        if (ci == -1)
        {
            perror("read");
            exit(-1);
        }

        command[ci-1] = '\0';

	  
	if (write(sock, command, sizeof(command)) < 0)
		perror("writing on stream socket");
	//getchar();
	int b;
	
	int ret = -1;
	while (ret != 0)
	{
			FD_ZERO(&readfds);

			FD_SET(sock, &readfds);
			ret = select(8, &readfds, NULL, NULL, &timeout);
			if (ret != 0)
			{
				b = read(sock, output, sizeof(output));
				//printf("b = %d", b);
				int c = write(STDOUT_FILENO, output, b);
				if (c < 0)
				{
					perror("writing output");
				}
			}
	}
	  
	}
	close(sock);
}
