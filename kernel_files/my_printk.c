#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/ktime.h>

asmlinkage void sys_my_printk(int pid, struct timespec start,
                                       struct timespec end) {
	printk("[project1] %d %lu.%09lu %lu.%09lu\n", pid,
	       start.tv_sec, start.tv_nsec,
               end.tv_sec, end.tv_nsec);
}
