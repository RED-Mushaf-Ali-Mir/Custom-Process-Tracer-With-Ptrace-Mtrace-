#include "TUI.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

int pidByname(char* p_name){

    int fd[2];

    if (pipe(fd) == -1){
        printf ("\nInternal Error -->Pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid == 0){ //child
        close (fd[0]);

        dup2(fd[1] , STDOUT_FILENO);

        close (fd[1]);

        execlp ("pgrep","pgrep","-n",p_name ,NULL); //oldest pid retuend -n as alone pgrep return mulitple pid

        perror ("Internal Error -->exec");
        return 1;

    }else {
        close(fd[1]);
        char p_pid[15];
        int char_read = read (fd[0] ,p_pid  , sizeof(p_pid) - 1);

        if (char_read == 0){
            p_pid [char_read] = '\0';
            printf ("\nNo Process With Such Name");
            return 0;
        }
        close(fd[0]);
        wait(NULL);

        return atoi(p_pid);

    }

    return 0;
}

pid_t RenderMenu(){
    //currently basic implmenation no TUI yet will implemnt later first we make core elements work like logging and Syscalls
    char choice;
    int t_pid = 0; 
    char p_name [20];
    pid_t tracee_id;

    while (1){
        system("clear");
        printf ("\n\nFor tracing With PID select 1 OR By Name select 2");
        scanf ("%c" , &choice);

        switch (choice){
            case '1':

            printf ("\nEnter the tracee id to Trace");
            scanf ("%i" , &t_pid);

            return (pid_t) t_pid;

            break;

            case '2':

            printf ("\nEnter the name of Process to trace");
            scanf("%s",&p_name);

            t_pid =pidByname(p_name);

            return (pid_t) t_pid;

            break;

            default:
                continue;

        }

    }


}