#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <linux/vmstat.h>
#include <linux/swap.h>
#include <linux/sysinfo.h>

struct memory_snapshot {
    unsigned long total_pages;      
    unsigned long free_pages;      
    unsigned long cached_pages;     
    unsigned long active_pages;     
    unsigned long inactive_pages;   
    unsigned long swap_pages;       
};

static void get_memory_snapshot(struct memory_snapshot *snapshot) {
    struct sysinfo i;

    si_meminfo(&i);

    snapshot->total_pages = i.totalram;                     
    snapshot->free_pages = i.freeram;                       
    snapshot->cached_pages = global_node_page_state(NR_FILE_PAGES); 

    snapshot->active_pages = global_node_page_state(NR_ACTIVE_ANON) +
                             global_node_page_state(NR_ACTIVE_FILE); 
    snapshot->inactive_pages = global_node_page_state(NR_INACTIVE_ANON) +
                               global_node_page_state(NR_INACTIVE_FILE); 

 
    snapshot->swap_pages = i.totalswap - i.freeswap;           


}

SYSCALL_DEFINE1(zamora_capture_memory_snapshot, struct memory_snapshot __user *, user_snapshot) {
    pr_info("Syscall zamora_capture_memory_snapshot invocada\n");

    struct memory_snapshot snapshot;

    // Llama a la función para recopilar el estado de la memoria
    get_memory_snapshot(&snapshot);

    // Copia los datos al espacio de usuario
    if (copy_to_user(user_snapshot, &snapshot, sizeof(snapshot))) {
        pr_err("Error: copy_to_user falló\n");
        return -EFAULT;
    }

    pr_info("Syscall zamora_capture_memory_snapshot completada\n");
    return 0;
}
