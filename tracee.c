
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
printf ("my pid : %d\n" , getpid());
sleep(10);
    while (1) {
        sleep(5);
        getpid();
        getppid();
    }
}
