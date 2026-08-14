#include <stdio.h>
#include <signal.h>
#include <sys/types.h>

pid_t PID;
pid_t checkPID(){
    printf("Please give the number of process to search the string for: ");
    scanf("%i", &PID);
    return PID;
}

int main(void){
    pid_t pid = checkPID();
}