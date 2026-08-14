#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdlib.h>

pid_t checkPID(){
    pid_t PID;
    printf("Please give the number of process to search the string for: ");
    scanf("%i", &PID);
    return PID;
}

bool proccessExists(pid_t pid){
    if(kill(pid, 0)==0) {
        return true;
    } else {
        return false;
    }
}

char* tryOpenAndReadMapsFile(pid_t pid){
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/maps", pid);
    FILE* file = fopen(path, "r");
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *content = malloc(size + 1);
    fread(content, 1, size, file);
    content[size]='\0';
    fclose(file);
    
    return content;
}

int main(void){
    pid_t pid = checkPID();
    if(!proccessExists(pid)){
        printf("Process doesn't exist");
        return 1;
    }
    printf("Process exists");
    char *content = tryOpenAndReadMapsFile(pid);
    printf(content);
    free(content);
}