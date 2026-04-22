#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <sys/user.h>

void do_tracer (pid_t tracee_pid){
    int  status;
    struct user_regs_struct regs;

    if (ptrace(PTRACE_ATTACH,tracee_pid , NULL , NULL ) == -1){
        perror ("ptrace PTRACE_ATTACH");
        return;
    }else {
        printf ("\nTracee Attached Successfully");
    }
    waitpid (tracee_pid , &status , 0);
    if (WIFSTOPPED(status)){
        printf ("\nChild Stopped , PID : %d " ,tracee_pid);
        while (1){
            if (ptrace(PTRACE_GETREGS, tracee_pid , NULL , &regs) == -1){
                perror ("ptrace GETREGS");
                exit(1);
            }
            printf ("\nTrace syscall  orig_rax = %lld ", regs.orig_rax);
            ptrace(PTRACE_SYSCALL , tracee_pid , NULL, NULL);
            waitpid(tracee_pid , &status , 0);
            if (WIFEXITED (status)){
                printf ("\nChild Exited with status %d \n" , WEXITSTATUS(status));
                break;
            }
        }
    }

}

int main (int argc , char* argv[]){
    pid_t tracee_pid;

    if (argc != 2){
        fprintf (stderr,"\nUsage %s <pid of program to debug>" , argv[0]);
        exit(1);
    } 

    tracee_pid = atoi (argv[1]);
    do_tracer (tracee_pid);

return 0;}
