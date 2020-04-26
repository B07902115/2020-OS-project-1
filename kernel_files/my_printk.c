#include <linux/linkage.h>
#include <linux/kernel.h>

asmlinkage void sys_my_printk(int pid,
	unsigned long ss, unsigned long sn,
	unsigned long es, unsigned long en){
	printk("[project1] %d %lu.%lu %lu.%lu\n", pid,
	       ss, sn, es, en);
}
