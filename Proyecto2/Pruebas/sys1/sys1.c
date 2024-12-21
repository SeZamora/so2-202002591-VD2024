#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <string.h>

struct memory_snapshot {
    unsigned long total_pages;      
    unsigned long free_pages;       
    unsigned long cached_pages;     
    unsigned long active_pages;     
    unsigned long inactive_pages;   
    unsigned long swap_pages;       
};

#define SYSCALL_CAPTURE_MEMORY_SNAPSHOT 551  
int main() {
    struct memory_snapshot snapshot;
    int result;

    // Invoca la syscall
    result = syscall(SYSCALL_CAPTURE_MEMORY_SNAPSHOT, &snapshot);

    if (result < 0) {
        printf("Error al invocar la syscall: %s\n", strerror(errno));
        return 1;
    }

    // Imprime los datos del snapshot
    printf("===== Estado actual de la memoria =====\n");
    printf("Total de páginas:       %lu\n", snapshot.total_pages);
    printf("Páginas libres:         %lu\n", snapshot.free_pages);
    printf("Páginas en caché:       %lu\n", snapshot.cached_pages);
    printf("Páginas activas:        %lu\n", snapshot.active_pages);
    printf("Páginas inactivas:      %lu\n", snapshot.inactive_pages);
    printf("Páginas de swap usadas: %lu\n", snapshot.swap_pages);
    printf("========================================\n");

    return 0;
}
