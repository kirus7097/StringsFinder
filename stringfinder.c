#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/types.h>

static pid_t checkPID() {
    pid_t pid;
    printf("Please give the PID of process: ");
    if (scanf("%d", &pid) != 1) {
        printf("Invalid PID\n");
        return -1;
    }
    return pid;
}

static bool processExists(pid_t pid) {
    if (kill(pid, 0)==0) return true;
    else return false;
}

static char* tryOpenAndReadMapsFile(pid_t pid) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/maps", pid);
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        perror("fopen");
        return NULL;
    }

    char content[8192];
    while (true) {
        if (fgets(content, sizeof(content), file) == NULL) break;
        printf("%s", content);
    }
}

int main(void) {
    pid_t pid = checkPID();
    if (!processExists(pid)) {
        printf("PID does not exist\n");
        return 1;
    }
    printf("PID exists\n");
    tryOpenAndReadMapsFile(pid);
}
