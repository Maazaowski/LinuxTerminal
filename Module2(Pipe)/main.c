#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

struct Processes {

    pid_t PID;
    BOOL status;
    char Pname[1000];
    time_t start_time;
    time_t end_time;
    time_t elapsed_time = 0;
};



int main () {
   struct Processes process_list[512];
   char cd[8] = "maaz:~$ ";
   char error[1000];
   int i = 0;
   int pid = -1;
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
                    char *path[1000];

                    char *arr[1000];
                    char *sl = strtok(command, "/");
                    int i = 0;


                    char *com = c;
                    while (c != NULL)
                    {
                        strcat(path, c);
                        strcat(path, " ");
                        c = strtok(NULL, " ");
                    }



                    char *argv[3] = {com,c, NULL};
                    pid = fork();

                    if (pid == 0)
                    {
                        int i = execvp(com, argv);
                        if (i < 0)
                        {
                            int p = execv(com, argv);
                            if (p < 0)
                            {
                                perror("run");
                            }
                            else{
                                time_t raw_time;
                                struct tm *timeinfo;
                                time (&raw_time);
                                timeinfo = localtime(&raw_time);
                                process_list[i].PID = getpid();
                                strcpy(process_list[i].Pname, com);
                                process_list[i].status = TRUE;
                                process_list[i].start_time = time(NULL);
                                process_list[i].end_time = NULL;
                                process_list[i].elapsed_time = 0;

                            }

                        }
                        else{
                            time_t raw_time;
                            struct tm *timeinfo;
                            time (&raw_time);
                            timeinfo = localtime(&raw_time);
                            process_list[i].PID = getpid();
                            strcpy(process_list[i].Pname, com);
                            process_list[i].status = TRUE;
                            process_list[i].start_time = time(NULL);
                            process_list[i].end_time = NULL;
                            process_list[i].elapsed_time = 0;

                        }
                    }

                    wait(2);
                }

            }
           else if (strcmp(c, "list") == 0){

                    for (int j = 0; j < sizeof(process_list); j++)
                    {
                        if (process_list[j].PID != 0){
                            printf("Process ID: %d\n", process_list[j].PID);
                            printf("Process Name: %s\n", process_list[j].Pname);
                            printf("Status: %d\n", process_list[j].status);
                            printf("Start Time: %s", asctime(localtime(&process_list[j].start_time)));
                            if (process_list[j].status)
                            printf("End Time: %s\n\n", asctime(process_list[j].end_time));
                            else
                            printf("End Time: %s\n\n", asctime(localtime(&process_list[j].end_time)));

                        }
                        else{
                            break;
                        }
                    }

            }

            else if (strcmp(c, "kill") == 0 && pid == 0)
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
                                process_list[j].status = FALSE;
                                process_list[j].end_time = time(NULL);
                                process_list[j].elapsed_time = process_list[j].end_time - process.list[j].start_time;
                                printf("Killed\n");

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
