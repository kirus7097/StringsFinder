#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <inttypes.h>

typedef struct {
    uintptr_t start;
    uintptr_t end;
    char permissions[5];
} MemoryRegion;

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

static void tryOpenAndReadMapsFile(pid_t pid) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/maps", pid);
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        perror("fopen");
        return;
    }

    char content[8192];
    unsigned int region_count = 0;
    MemoryRegion region[100];

    while (true) {
        MemoryRegion current;

        if (fgets(content, sizeof(content), file) == NULL) break;
        if (sscanf(content, "%" SCNxPTR "-%" SCNxPTR " %4s", &region[region_count].start, &region[region_count].end, region[region_count].permissions) != 3) {
            fclose(file);
            printf("Failed to read addresses or parse them");
            return;
        }

        if (region[region_count].permissions[0] == 'r') {
            printf("Start address: %" PRIxPTR "\n", region[region_count].start);
            printf("End address: %" PRIxPTR "\n", region[region_count].end);
            printf("Permissions: %s\n", region[region_count].permissions);
            region_count++;
        }
    }
    fclose(file);
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
