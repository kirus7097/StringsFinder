#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

typedef struct {
    uintptr_t start;
    uintptr_t end;
    char permissions[5];
} MemoryRegion;

static pid_t checkPID(void) {
    pid_t pid;

    printf("Please give the PID of process: ");

    if (scanf("%d", &pid) != 1) {
        printf("Invalid PID\n");
        return -1;
    }

    return pid;
}

static bool processExists(pid_t pid) {
    if (pid <= 0) {
        return false;
    }
    if (kill(pid, 0) == 0) {
        return true;
    }
    if (errno == EPERM) {
        return true;
    }
    return false;
}

static unsigned int tryOpenAndReadMapsFile(pid_t pid, MemoryRegion regions[], unsigned int max_regions) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/maps", pid);
    FILE *file = fopen(path, "r");

    if (file == NULL) {
        perror("fopen");
        return 0;
    }

    char content[8192];
    unsigned int region_count = 0;

    while (fgets(content, sizeof(content), file) != NULL) {
        if (region_count >= max_regions) {
            break;
        }
        if (sscanf(content, "%" SCNxPTR "-%" SCNxPTR " %4s", &regions[region_count].start, &regions[region_count].end, regions[region_count].permissions) != 3) {
            continue;
        }
        if (regions[region_count].permissions[0] == 'r') {
            region_count++;
        }
    }

    fclose(file);
    return region_count;
}

static int readProcessMemory(pid_t pid, MemoryRegion region, uintptr_t region_size, char search[]) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/mem", pid);
    int file = open(path, O_RDONLY);

    if (file == -1) {
        perror("open");
        return -1;
    }

    char content[8192];
    uintptr_t remaining = region_size;

    off_t seek = lseek(file, (off_t)region.start, SEEK_SET);

    if (seek == (off_t)-1) {
        perror("lseek");
        close(file);
        return -1;
    }

    while (remaining > 0) {
        size_t bytes_to_read;

        if (remaining > sizeof(content)) {
            bytes_to_read = sizeof(content);
        } else {
            bytes_to_read = remaining;
        }
        ssize_t bytes_read = read(file, content, bytes_to_read, search[]);
        if (bytes_read == -1) {
            perror("read");
            close(file);
            return -1;
        }
        if (bytes_read == 0) {
            break;
        }

        printf("\n");
        remaining -= bytes_read;
    }

    close(file);
    return 0;
}

static void searchBuffer(char content[], ssize_t bytes_read, char search[]) {

}

int main(void) {
    pid_t pid = checkPID();
    char search[256];

    if (pid == -1) {
        return 1;
    }

    if (!processExists(pid)) {
        printf("PID does not exist\n");
        return 1;
    }

    printf("PID exists\n");
    printf("Please provide text to search for: ");
    if (scanf("%255s", search) != 1) {
        printf("Invalid text\n");
        return -1;
    }
    printf("Search for: %s\n", search);

    MemoryRegion regions[100];
    unsigned int region_count = tryOpenAndReadMapsFile(pid, regions, 100);
    printf("READ MEMORY:\n\n");

    for (unsigned int i = 0; i < region_count; i++) {
        uintptr_t region_size = regions[i].end - regions[i].start;
        readProcessMemory(pid, regions[i], region_size, search);
    }
    return 0;
}
