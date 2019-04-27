#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>



struct Processes {

    pid_t PID;
    int status;
    char Pname[1000];
    time_t start_time;
    time_t end_time;
    int elapsed_time;
};

struct Clients {
    char IPAddress[100];
    time_t start_time;
    time_t end_time;
    int connected;

};

pid_t tpid = 0;
int status;

void child_terminated() {
    pid_t pid = waitpid(-1, NULL, WNOHANG);
}

void terminalTermination() {
    pid_t pid = waitpid(-1, NULL, WNOHANG);

}

void sigterm_handler() {
    pid_t pid = waitpid(-1, NULL, WNOHANG);
}


int main () {

   struct Clients clients_list[100];
   //int length = 0;
   int clientno = 0;
   
   int processpipe[2];

   int ret = pipe(processpipe);

   if (ret == -1)
   {
       perror("pipe");
       exit(-1);
   }
   
  
   int sock, length;
   struct sockaddr_in server;
   int msgsock;
   char command[1024];
   int rval;
   int i;
   char error[1000];
   
   
   sock = socket(AF_INET, SOCK_STREAM, 0);
   if (sock < 0) {
		perror("opening stream socket");
		exit(1);
   }

    server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = 0;
	if (bind(sock, (struct sockaddr *) &server, sizeof(server))) {
		perror("binding stream socket");
		exit(1);
	}

    length = sizeof(server);
	if (getsockname(sock, (struct sockaddr *) &server, (socklen_t*) &length)) {
		perror("getting socket name");
		exit(1);
	}
    char s[100];
	int sc = sprintf(s, "Socket has port #%d\n", ntohs(server.sin_port));
    int retsc = write(STDOUT_FILENO, s, sc);
    if (retsc < 0)
    {
        perror("Socket write");
    }
	fflush(stdout);

    listen(sock, 5);
    

    do {
        struct sockaddr_in client;
        socklen_t c_size = sizeof(struct sockaddr_in);

		msgsock = accept(sock, (struct sockaddr *) &client, &c_size);
		if (msgsock == -1)
        {
			perror("accept");
            exit(-1);
        }
        char ip[15];

        inet_ntop(AF_INET, &client, ip, INET_ADDRSTRLEN);
        
        strcpy(clients_list[clientno].IPAddress, ip);
        clients_list[clientno].start_time = time(NULL);
        clients_list[clientno].connected = 1;
        clientno++;
        char buff1[100];
        write(STDOUT_FILENO, "\n", 1);
        write(STDOUT_FILENO, "Clients Connected!\n", sizeof("Clients Connected!\n"));
        for (int j = 0; j <= clientno && clients_list[j].connected; j++)
        {
            write(STDOUT_FILENO, clients_list[j].IPAddress, sizeof(ip));
            write(STDOUT_FILENO, "\n", 1);
            int d = sprintf(buff1, "Start Time: %s", asctime(localtime(&clients_list[j].start_time)));
            write(STDOUT_FILENO, buff1, d);
            int e = sprintf(buff1, "Elapsed Time: %d secs\n", (int) (time(NULL) - clients_list[j].start_time));
            int elapsed = write(STDOUT_FILENO, buff1, e);
            if (elapsed < 0)
            {
                perror("Elapsed write");
            }
            write(STDOUT_FILENO, "--------------------------\n", sizeof("--------------------------\n"));
        }

        int p = fork();
        if (p == 0)
        {
            signal(SIGCHLD, child_terminated);
            signal(SIGINT, terminalTermination);
            signal(SIGTERM, sigterm_handler);
            int i = 0;
            int pid = -1;
            struct Processes process_list[100];
            int size = sizeof(process_list) / sizeof(process_list[0]);
            for (int j = 0; j < size; j++)
            {
                process_list[j].PID = 0;
                process_list[j].status = 0;
                strcpy(process_list[j].Pname, "");
                process_list[j].start_time = NULL;
                process_list[j].end_time = NULL;
                process_list[j].elapsed_time = 0;
            }

            int ipc = sprintf(ip, "%s: ", ip);
		do {
                
			bzero(command, sizeof(command));
			if ((rval = read(msgsock, command, sizeof(command))) < 0)
            {
                write(STDOUT_FILENO, ip, ipc);
				perror("reading stream message");
            }

			int killpid = waitpid(-1, NULL, WNOHANG);

            if (killpid > 0)
            {
                for (int j = 0; j < size; j++)
                {
                    if (process_list[j].PID == killpid)
                    {
                        process_list[j].status = 0;
                        process_list[j].end_time = time(NULL);
                        process_list[j].elapsed_time = (int) process_list[j].end_time - process_list[j].start_time;
                        break;
                    }
                }
            }


			if (rval == 0)
            {
                write(STDOUT_FILENO, ip, ipc);
				int retec = write(STDOUT_FILENO, "Ending connection\n", sizeof("Ending connection\n"));
                for (int j = 0; j < size && process_list[j].status; j++)
                {   
                    kill(process_list[j].PID, SIGTERM);
                    waitpid(-1, NULL, WNOHANG);
                }
                if (retec < 0)
                {
                    
                    perror("Ending connection write");
                }
            }
			else{

            if (strcmp (command, "exit") == 0)
            {
            for (int j = 0; j < size; j++)
            {
                kill(process_list[j].PID, SIGTERM);
                pid_t pid = waitpid(-1, NULL, WNOHANG);
            }
            
            write(STDOUT_FILENO, ip, ipc);
			int retec = write(STDOUT_FILENO, "Ending connection\n", sizeof("Ending connection\n"));
            kill(getpid(), SIGTERM);
            exit(0);

            }
        else if (strcmp (command, "help") == 0)
        {
            write(msgsock, "add <num1> <num2> ... <numN> -- to add\n", sizeof("add <num1> <num2> ... <numN> -- to add\n"));
            write(msgsock, "mul <num1> <num2> ... <numN> -- to multiply\n", sizeof("mul <num1> <num2> ... <numN> -- to multiply\n"));
            write(msgsock, "div <num1> <num2> -- to divide two numbers\n", sizeof("div <num1> <num2> -- to divide two numbers\n"));
            write(msgsock, "sub <num1> <num2> ... <numN> -- to subtract\n", sizeof("sub <num1> <num2> ... <numN> -- to subtract\n"));
            write(msgsock, "run <process> -- to run the process\n", sizeof("run <process> -- to run the process\n"));
            write(msgsock, "list -- to list all the processes\n", sizeof("list -- to list all the processes\n"));
            write(msgsock, "kill <process> -- to kill the process\n", sizeof("kill <process> -- to kill the process\n"));
            write(msgsock, "exit -- to exit the program\n", sizeof("exit -- to exit the program\n"));
        }
        else
        {
            char* c = strtok(command, " ");

            if (strcmp (c, "add") == 0)
            {
                c = strtok(NULL, " ");
                if (c == NULL){
                    write(msgsock, "ERROR: Not enough arguments\n", sizeof("ERROR: Not enough arguments\n"));
                    continue;
                }
                int total = 0;
                while (c != NULL)
                {
                    total += atoi(c);
                    c = strtok(NULL, " ");
                }
                char tot[1000000];
                int t = sprintf(tot, "%d\n", total);
                write(msgsock, tot, t);

            }
            else if (strcmp (c, "mul") == 0)
            {
                c = strtok(NULL, " ");
                if (c == NULL){
                    write(msgsock, "ERROR: Not enough arguments\n", sizeof("ERROR: Not enough arguments\n"));
                    continue;
                }
                int total = 1;
                while (c != NULL)
                {
                    total *= atoi(c);
                    c = strtok(NULL, " ");
                }
                char tot[1000000];
                int t = sprintf(tot, "%d\n", total);
                write(msgsock, tot, t);

            }
            else if (strcmp (c, "div") == 0)
            {
                c = strtok(NULL, " ");
                if (c == NULL){
                    write(msgsock, "ERROR: Not enough arguments\n", sizeof("ERROR: Not enough arguments\n"));
                    continue;
                }
                int x = atoi(c);
                c = strtok(NULL, " ");
                if (c == NULL){
                    write(msgsock, "ERROR: Not enough arguments\n", sizeof("ERROR: Not enough arguments\n"));
                    continue;
                }
                int y = atoi(c);
                c = strtok(NULL, " ");
                if (c == NULL){
                    char tot[1000000];
                    int t = sprintf(tot, "%d\n", (x/y));
                    write(msgsock, tot, t);
                }
                else
                    write(msgsock, "ERROR: Too many arguments\n", sizeof("ERROR: Too many arguments\n"));

            }
            else if (strcmp (c, "sub") == 0)
            {
                c = strtok(NULL, " ");
                if (c == NULL){
                    write(msgsock, "ERROR: Not enough arguments\n", sizeof("ERROR: Not enough arguments\n"));
                    continue;
                }
                int total = atoi(c);
                c = strtok(NULL, " ");
                while (c != NULL)
                {
                    total = total - atoi(c);
                    c = strtok(NULL, " ");
                }
                char tot[1000000];
                int t = sprintf(tot, "%d\n", total);
                write(msgsock, tot, t);

            }
            else if (strcmp (c, "run") == 0)
            {
                c = strtok(NULL, " ");

                if (c == ' ' || c == NULL)
                {
                    write(msgsock, "Invalid arguments for run\n", sizeof("Invalid arguments for run\n"));
                }
                else{
                    char args[1000] = "";

                   // char *arr[1000];
                    //char *sl = strtok(command, "/");
                    


                    char *com = c;
                    c = strtok(NULL, " ");
                    while (c != NULL)
                    {

                        strcat(args, c);
                        strcat(args, " ");
                        c = strtok(NULL, " ");
                    }



                    char *argv[3] = {com,args, NULL};
                    pid = fork();

                    if (pid > 0)
                    {
                        time_t raw_time;
                        struct tm *timeinfo;
                        time (&raw_time);
                        timeinfo = localtime(&raw_time);
                        process_list[i].PID = pid;
                        //printf("%d\n", getpid());
                        //write(STDOUT_FILENO, "ERROR: Not enough arguments\n", sizeof("ERROR: Not enough arguments\n"))
                        strcpy(process_list[i].Pname, com);
                        process_list[i].status = 1;
                        process_list[i].start_time = time(NULL);
                        process_list[i].end_time = NULL;
                        process_list[i].elapsed_time = 0;
                        i++;
                        
                    }

                    if (pid == 0)
                    {
                        
                        
                        int a = execvp(com, argv);
                        if (a < 0)
                        {
                            int p = execv(com, argv);
                            if (p < 0)
                            {
                                perror("run");
                                kill(getppid(), SIGUSR1);
                            }
                            else{
                                
                                //kill(getppid(), SIGUSR2);
                                //i++;
                                //write(msgsock, "Executed Successfully\n", sizeof("Executed Successfully\n"));

                            }

                        }
                        else{
                            //write(msgsock, "Executed Successfully\n", sizeof("Executed Successfully\n"));
                            //kill(getppid(), SIGUSR2);
                            //i++;

                        }
                    }

                    //wait(2);
                }

            }
           else if (strcmp(c, "list") == 0){
			int bo = 0;

                    for (int j = 0; j < size; j++)
                    {
                        if (process_list[j].PID != 0){
			    bo = 1;
                            char buff1[10000];
                            int a = sprintf(buff1, "Process ID: %d\n", process_list[j].PID);
                            write(msgsock, buff1, a);
                            int b = sprintf(buff1, "Process Name: %s\n", process_list[j].Pname);
                            write(msgsock, buff1, b);
                            int c = sprintf(buff1, "Status: %d\n", process_list[j].status);
                            write(msgsock, buff1, c);
                         
                            int d = sprintf(buff1, "Start Time: %s", asctime(localtime(&process_list[j].start_time)));
                            write(msgsock, buff1, d);
            
                            if (process_list[j].status)
                            {
                                int e = sprintf(buff1, "Elapsed Time: %d secs\n\n", (int) (time(NULL) - process_list[j].start_time));
                                int elapsed = write(msgsock, buff1, e);
                                if (elapsed < 0)
                                {
                                    perror("Elapsed write");
                                }
                            }
                            else {
                                int e = sprintf(buff1, "End Time: %s\n", asctime(localtime(&process_list[j].end_time)));
                                write(msgsock, buff1, e);
                                int f = sprintf(buff1, "Elapsed Time: %d secs\n\n", (int) (process_list[j].end_time - process_list[j].start_time));
                                write(msgsock, buff1, f);
                            }
                        }
                        else{
                            break;
                        }
                    }
		if (bo == 0)
		{
			int cret = write(msgsock, "No processes in list\n", sizeof("No processes in list\n"));
		}

            }

            else if (strcmp(c, "kill") == 0)
            {
                
                
                c = strtok(NULL, " ");

                if (c == ' ' || c == NULL)
                {
                    write(msgsock, "Invalid arguments for kill\n", sizeof("Invalid arguments for kill\n"));
                }
                else{
                    char path[1000] = "";
                   /* while (c != NULL)
                    {
                        strcat(path, c);
                        strcat(path, " ");
                        c = strtok(NULL, " ");
                    }
*/
                    int b = 0;

                    for (int j = 0; j < size && process_list[j].PID != 0; j++)
                    {
                        int q = write(STDOUT_FILENO, path, sizeof(path));
                            if (q < 0)
                            {
                                perror("write path");
                            }

                        if (process_list[j].status)
                        {

                            int prodID = atoi(path);
                            int prodID2 = (int) process_list[j].PID;
                            int q = write(STDOUT_FILENO, path, sizeof(path));
                            if (q < 0)
                            {
                                perror("write path");
                            }
                            if (prodID == prodID2 || strcmp(process_list[j].Pname, c) == 0)
                            {
                                int err = kill(process_list[j].PID, SIGTERM);
                                if (err < 0)
                                    perror("kill");
                                else{
                                    int killed = write(msgsock, "Killed\n", sizeof("Killed\n"));
                                    process_list[j].status = 0;
                                    process_list[j].end_time = time(NULL);
                                    process_list[j].elapsed_time = (int) process_list[j].end_time - process_list[j].start_time;
                                    if (killed < 0)
                                    {
                                        perror("Killed write");
                                    }
                                    b = 1;
                                    //wait(NULL);
                                }
                               
                                //exit(0);
                                

                                break;

                            }
                            else{
                                
                                }
                            }
                        }

                        if (b == 0)
                        {
                            int cannot = write(msgsock, "Could not kill\n", sizeof("Could not kill\n"));
                            if (cannot < 0)
                            {
                                perror("Cannot kill write");
                            }
                        }
                    }

                }

            else {

                char msg[1000000];
                int t = sprintf(msg, "%s: command not found\n", command);
                int notfound = write(msgsock, msg, t);
                if (notfound < 0)
                {
                    perror("Not found write");
                }
            }
        }


            }



				//printf("-->%s\n", command);
		 }while (rval != 0);
        }
		
	} while (1);
   close(msgsock);



}
