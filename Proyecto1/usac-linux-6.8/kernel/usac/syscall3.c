#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>

#include <linux/gfp.h>
#include <linux/unistd.h>

struct io_stats
{
    unsigned long bytes_read;
    unsigned long bytes_written;
    unsigned long bytes_read_disk;
    unsigned long bytes_written_disk;
    unsigned long io_wait_time;
};

SYSCALL_DEFINE2(zamora_get_io_throttle, pid_t, pid, struct io_stats __user *,stats){
    struct task_struct *task;
    struct io_stats kstats;

    task = find_task_by_vpid(pid);
    if(!task){
        printk(KERN_ERR "zamora_get_io_throttle: PID %d no encontrado\n", pid);
        return -ESRCH;
    }

    task_lock(task);

    kstats.bytes_read = task->ioac.rchar;
    kstats.bytes_written = task->ioac.wchar;

    kstats.bytes_read_disk = task->ioac.read_bytes;
    kstats.bytes_written_disk = task->ioac.write_bytes;

    task_unlock(task);
    
    if (copy_to_user(stats, &kstats, sizeof(struct io_stats))) {
        printk(KERN_ERR "Error: copy_to_user falló\n");
        return -EFAULT;
    }
    return 0;
};
