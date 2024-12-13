#include <linux/mm.h>
#include <linux/gfp.h>
#include <linux/uaccess.h>
#include <linux/syscalls.h>
#include <asm/unistd.h>


#include <linux/time.h>
#include <linux/ktime.h>

#include "syscall2.h"

struct syscall_usage {
	unsigned long count;
	struct timespec64 time_last_used;
};

#define MAX_SYS_CALLS 1024
static struct syscall_usage *syscall_counters;


static int init_syscall_counters(void) {
	syscall_counters = kzalloc(sizeof(struct syscall_usage)*MAX_SYS_CALLS,GFP_KERNEL);

	return syscall_counters ? 0 : -ENOMEM;
}

void track_syscall(int syscall_id) {

	struct timespec64 now;

	if (!syscall_counters || syscall_id >= MAX_SYS_CALLS) {
		return;
	}

	syscall_counters[syscall_id].count++;
	ktime_get_real_ts64(&now);
	syscall_counters[syscall_id].time_last_used = now;
	
	//TODO Comprobar si esto funciona???
	//ktime_get_real_ts64(&syscall_counters[syscall_id].time_last_used);
}




SYSCALL_DEFINE1(zamora_track_syscall_usage, struct syscall_usage __user*, statistics) {
	if (!syscall_counters) {
		if (init_syscall_counters() != 0) {
			return -ENOMEM;
		}
	}

	int resultadoCopia = copy_to_user(statistics, syscall_counters, sizeof(struct syscall_usage)*MAX_SYS_CALLS);

	if (resultadoCopia) {
		return -EFAULT;
	}
	return 0;
}

