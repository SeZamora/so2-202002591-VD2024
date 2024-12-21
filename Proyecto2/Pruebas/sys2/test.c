#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#define MAX_SYS_CALLS 1024

struct syscall_usage {
    unsigned long count;
    struct timespec time_last_used;
};

#define SYSCALL_ZAMORA_TRACK_USAGE 552  

int main() {
    struct syscall_usage statistics[MAX_SYS_CALLS];
    int result;


    result = syscall(SYSCALL_ZAMORA_TRACK_USAGE, statistics);

    if (result < 0) {
        printf("Error al invocar la syscall: %s\n", strerror(errno));
        return 1;
    }

    
    for (int i = 0; i < 4; i++) {


        printf("-----------------------------------------\n");
        printf("Syscall ID %d:\n", i);
        printf("  Count: %lu\n", statistics[i].count);
        printf("  Last Used: %lld.%09ld\n", 
                (long long)statistics[i].time_last_used.tv_sec, 
                statistics[i].time_last_used.tv_nsec);
        printf("-----------------------------------------\n");
    }

    return 0;
}
