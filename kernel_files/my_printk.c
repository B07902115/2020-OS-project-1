#include <linux/linkage.h>
#include <linux/kernel.h>
#include <time.h>

asmlinkage int sys_my_printk(int pid, struct timespec start, struct timespec end){
	printk("[project1] %d %lu.%lu %lu.%lu\n", pid,
		   start.tv_sec, start.tv_nsec, end.tv_sec, end.tv_nsec);
}