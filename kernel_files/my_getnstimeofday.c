#include <linux/ktime.h>
#include <linux/timekeeping.h>

asmlinkage void sys_my_getnstimeofday(struct timespec *t) {
	getnstimeofday(t);
}