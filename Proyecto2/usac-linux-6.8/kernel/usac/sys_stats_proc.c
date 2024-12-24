#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <linux/types.h> 
#include <linux/syscalls.h>


struct process_mem_stats {
    pid_t pid;
    unsigned long reserved_kb;   // Memoria reservada en KB
    unsigned long committed_kb;  // Memoria comprometida en KB
    unsigned long committed_percent; // % de memoria comprometida
    int oom_score;
};

SYSCALL_DEFINE2(zamora_mem_stats_process, pid_t, pid, struct process_mem_stats __user *, stats) {
    struct task_struct *task;
    struct mm_struct *mm;
    struct process_mem_stats kstats;
    int count = 0;

    // Caso: PID > 0, recolectar datos de un único proceso
    if (pid > 0) {
        task = find_task_by_vpid(pid);
        if (!task)
            return -ESRCH;

        mm = get_task_mm(task);
        if (!mm)
            return -EFAULT;

        // Recolectar estadísticas de memoria
        kstats.pid = pid;
        kstats.reserved_kb = mm->total_vm << (PAGE_SHIFT - 10);
        kstats.committed_kb = get_mm_rss(mm) << (PAGE_SHIFT - 10);
        kstats.committed_percent = (kstats.committed_kb * 100) / kstats.reserved_kb;
        kstats.oom_score = task->signal->oom_score_adj;

        mmput(mm);

        if (copy_to_user(stats, &kstats, sizeof(kstats)))
            return -EFAULT;

        return 1;
    }

    // Caso: PID = 0, devolver datos de todos los procesos
    rcu_read_lock();
    for_each_process(task) {
        mm = get_task_mm(task);
        if (!mm)
            continue;

        // Recolectar estadísticas del proceso actual
        kstats.pid = task_pid_nr(task);
        kstats.reserved_kb = mm->total_vm << (PAGE_SHIFT - 10);
        kstats.committed_kb = get_mm_rss(mm) << (PAGE_SHIFT - 10);
        kstats.committed_percent = (kstats.committed_kb * 100) / kstats.reserved_kb;
        kstats.oom_score = task->signal->oom_score_adj;

        mmput(mm);

        if (copy_to_user(stats + count, &kstats, sizeof(kstats))) {
            rcu_read_unlock();
            return -EFAULT;
        }

        count++;
    }
    rcu_read_unlock();

    return count; 
}
