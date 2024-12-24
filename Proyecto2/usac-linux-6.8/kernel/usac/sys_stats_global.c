#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <linux/types.h>
#include <linux/syscalls.h>

struct system_mem_stats {
    unsigned long total_reserved_mb;   // Memoria total reservada en MB
    unsigned long total_committed_mb;  // Memoria total comprometida en MB
};

SYSCALL_DEFINE1(zamora_mem_stats_global, struct system_mem_stats __user *, stats) {
    struct task_struct *task;
    struct mm_struct *mm;
    struct system_mem_stats kstats = {0, 0};
    unsigned long reserved_pages = 0;
    unsigned long committed_pages = 0;

    // Iterar sobre todos los procesos
    rcu_read_lock(); 
    for_each_process(task) {
        mm = get_task_mm(task);
        if (!mm)
            continue;

        // Sumar la memoria reservada y comprometida de cada proceso
        reserved_pages += mm->total_vm;
        committed_pages += get_mm_rss(mm);

        mmput(mm);
    rcu_read_unlock();

    kstats.total_reserved_mb = (reserved_pages << PAGE_SHIFT) >> 20;
    kstats.total_committed_mb = (committed_pages << PAGE_SHIFT) >> 20;

    // Copiar las estadísticas al espacio de usuario
    if (copy_to_user(stats, &kstats, sizeof(kstats)))
        return -EFAULT;

    return 0;
}
