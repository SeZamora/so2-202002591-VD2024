#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h> 

struct io_stats {
    unsigned long bytes_read;
    unsigned long bytes_written;
    unsigned long bytes_read_disk;
    unsigned long bytes_written_disk;
    unsigned long io_wait_time;
};

#define SYSCALL_ZAMORA_GET_IO_THROTTLE 553 

int main() {
    
    pid_t pid;
    struct io_stats stats;

    // Solicitar el PID del proceso
    printf("Ingrese el PID del proceso para obtener estadísticas de I/O: ");
    if (scanf("%d", &pid) != 1) {
        printf("Error: Entrada inválida\n");
        return 1;
    }

    // Llama a la syscall
    int result = syscall(SYSCALL_ZAMORA_GET_IO_THROTTLE, pid, &stats);

    if (result < 0) {
        printf("Error al invocar la syscall: %s\n", strerror(errno));
        return 1;
    }

    // Muestra las estadísticas de I/O
    printf("===== Estadísticas de I/O para PID %d =====\n", pid);
    printf("Bytes leídos:              %lu\n", stats.bytes_read);
    printf("Bytes escritos:            %lu\n", stats.bytes_written);
    printf("Bytes leídos del disco:    %lu\n", stats.bytes_read_disk);
    printf("Bytes escritos al disco:   %lu\n", stats.bytes_written_disk);
    printf("Tiempo de espera en I/O:   %lu\n", stats.io_wait_time);
    printf("==========================================\n");

    return 0;
}
