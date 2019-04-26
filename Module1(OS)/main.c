#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>


struct Processes {

    pid_t PID;
    int status;
    char Pname[1000];
    time_t start_time;
    time_t end_time;
    int elapsed_time;
};

struct Processes process_list[100];
int i = 0;

void successfulProcess() {
    i++;
    char buf[1000];
    sprintf(buf, "Process executed: %d\n", getpid());
    write(STDOUT_FILENO, buf, 26);
}

void unsuccessfulProcess() {

       process_list[i].PID = 0;
       process_list[i].status = 0;
       strcpy(process_list[i].Pname, "");
       process_list[i].start_time = NULL;
       process_list[i].end_time = NULL;
       process_list[i].elapsed_time = 0;
       char buf[1000];
       sprintf(buf, "Process not executed: %d\n", getpid());
       write(STDOUT_FILENO, buf, 29);


}



int main () {
  
   int size = sizeof(process_list) / sizeof(process_list[0]);
   char cd[8] = "maaz:~$ ";
   char error[1000];
   int pid = -1;
   
   for (int j = 0; j < size; j++)
   {
       process_list[j].PID = 0;
       process_list[j].status = 0;
       strcpy(process_list[j].Pname, "");
       process_list[j].start_time = NULL;
       process_list[j].end_time = NULL;
       process_list[j].elapsed_time = 0;
   }

   while (1)
   {
        int a = write(STDOUT_FILENO, cd, sizeof(cd));
        if (a == -1)
        {
            perror("write");
            exit(-1);
        }
        char command[100];
        int c = read(STDIN_FILENO, command, sizeof(command));

        if (c == -1)
        {
            perror("read");
            exit(-1);
        }

        command[c-1] = '\0';

        if (strcmp (command, "exit") == 0)
        {
            exit(0);

        }
        else if (strcmp (command, "help") == 0)
        {
            write(STDOUT_FILENO, "add <num1> <num2> ... <numN> -- to add\n", sizeof("add <num1> <num2> ... <numN> -- to add\n"));
            write(STDOUT_FILENO, "mul <num1> <num2> ... <numN> -- to multiply\n", sizeof("mul <num1> <num2> ... <numN> -- to multiply\n"));
            write(STDOUT_FILENO, "div <num1> <num2> -- to divide two numbers\n", sizeof("div <num1> <num2> -- to divide two numbers\n"));
            write(STDOUT_FILENO, "sub <num1> <num2> ... <numN> -- to subtract\n", sizeof("sub <num1> <num2> ... <numN> -- to subtract\n"));
            write(STDOUT_FILENO, "run <process> -- to run the process\n", sizeof("run <process> -- to run the process\n"));
            write(STDOUT_FILENO, "list -- to list all the processes\n", sizeof("list -- to list all the processes\n"));
            write(STDOUT_FILENO, "kill <process> -- to kill the process\n", sizeof("kill <process> -- to kill the process\n"));
            write(STDOUT_FILENO, "exit -- to exit the program\n", sizeof("exit -- to exit the program\n"));
        }
        else
        {
            char* c = strtok(command, " ");

            if (strcmp (c, "add") == 0)
            {
                c = strtok(NULL, " ");
                if (c == NULL){
                    write(STDOUT_FILENO, "ERROR: Not enough arguments\n", sizeof("ERROR: Not enough arguments\n"));
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
                write(STDOUT_FILENO, tot, t);

            }
            else if (strcmp (c, "mul") == 0)
            {
                c = strtok(NULL, " ");
                if (c == NULL){
                    write(STDOUT_FILENO, "ERROR: Not enough arguments\n", sizeof("ERROR: Not enough arguments\n"));
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
                write(STDOUT_FILENO, tot, t);

            }
            else if (strcmp (c, "div") == 0)
            {
                c = strtok(NULL, " ");
                if (c == NULL){
                    write(STDOUT_FILENO, "ERROR: Not enough arguments\n", sizeof("ERROR: Not enough arguments\n"));
                    continue;
                }
                int x = atoi(c);
                c = strtok(NULL, " ");
                if (c == NULL){
                    write(STDOUT_FILENO, "ERROR: Not enough arguments\n", sizeof("ERROR: Not enough arguments\n"));
                    continue;
                }
                int y = atoi(c);
                c = strtok(NULL, " ");
                if (c == NULL){
                    char tot[1000000];
                    int t = sprintf(tot, "%d\n", (x/y));
                    write(STDOUT_FILENO, tot, t);
                }
                else
                    write(STDOUT_FILENO, "ERROR: Too many arguments\n", sizeof("ERROR: Too many arguments\n"));

            }
            else if (strcmp (c, "sub") == 0)
            {
                c = strtok(NULL, " ");
                if (c == NULL){
                    write(STDOUT_FILENO, "ERROR: Not enough arguments\n", sizeof("ERROR: Not enough arguments\n"));
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
                write(STDOUT_FILENO, tot, t);

            }
            else if (strcmp (c, "run") == 0)
            {
                c = strtok(NULL, " ");

                if (c == ' ' || c == NULL)
                {
                    write(STDOUT_FILENO, "Invalid arguments for run\n", sizeof("Invalid arguments for run\n"));
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
                    }

                    if (pid == 0)
                    {
                        signal(SIGUSR1, unsuccessfulProcess);
                        signal(SIGUSR2, successfulProcess);
                        int a = execvp(com, argv);
                        if (a < 0)
                        {
                            int p = execv(com, argv);
                            if (p < 0)
                            {
                                perror("run");
                                kill(getpid(), SIGUSR1);
                            }
                            else{
                                
                                kill(getpid(), SIGUSR2);
                                //i++;

                            }

                        }
                        else{
                            
                            kill(getpid(), SIGUSR2);
                            //i++;

                        }
                    }

                    //wait(2);
                }

            }
           else if (strcmp(c, "list") == 0){

                    for (int j = 0; j < size; j++)
                    {
                        if (process_list[j].PID != 0){
                            char buff1[10000];
                            int a = sprintf(buff1, "Process ID: %d\n", process_list[j].PID);
                            write(STDOUT_FILENO, buff1, a);
                            int b = sprintf(buff1, "Process Name: %s\n", process_list[j].Pname);
                            write(STDOUT_FILENO, buff1, b);
                            int c = sprintf(buff1, "Status: %d\n", process_list[j].status);
                            write(STDOUT_FILENO, buff1, c);
                         
                            int d = sprintf(buff1, "Start Time: %s", asctime(localtime(&process_list[j].start_time)));
                            write(STDOUT_FILENO, buff1, d);
            
                            if (process_list[j].status)
                            {
                                int e = sprintf(buff1, "End Time: %s\n\n", asctime(process_list[j].end_time));
                                write(STDOUT_FILENO, buff1, e);
                            }
                            else {
                                int e = sprintf(buff1, "End Time: %s\n\n", asctime(localtime(&process_list[j].end_time)));
                                write(STDOUT_FILENO, buff1, e);
                            }
                        }
                        else{
                            break;
                        }
                    }

            }

            else if (strcmp(c, "kill") == 0)
            {
                c = strtok(NULL, " ");

                if (c == ' ' || c == NULL)
                {
                    printf("Invalid arguments for kill\n");
                }
                else{
                    char path[1000] = "";
                    while (c != NULL)
                    {
                        strcat(path, c);
                        strcat(path, " ");
                        c = strtok(NULL, " ");
                    }


                    for (int j = 0; j < sizeof(process_list) && process_list[j].PID != 0; j++)
                    {

                        if (process_list[j].status)
                        {

                            int prodID = atoi(path);
                            int prodID2 = (int) process_list[j].PID;
                            if (prodID == prodID2 || strcmp(process_list[j].Pname, path) == 0)
                            {
                                process_list[j].status = 0;
                                process_list[j].end_time = time(NULL);
                                process_list[j].elapsed_time = (int) process_list[j].end_time - process_list[j].start_time;
                                printf("Killed\n");

                                kill(process_list[j].PID, SIGTERM);
                                exit(0);

                                break;

                            }
                            else{
                                printf("Could not kill\n");
                                break;
                                }
                            }
                        }
                    }

                }

            else {

                char msg[1000000];
                int t = sprintf(msg, "%s: command not found\n", command);
                write(STDOUT_FILENO, msg, t);
            }
        }

    }


}
