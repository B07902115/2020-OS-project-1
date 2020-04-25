#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <time.h>
#include <string.h>
#include <sched.h>

#define STRSIZE 256
#define FIFO 0
#define RR 1
#define SJF 2
#define PSJF 3
#define PRINTK 315

struct procData {
	char name[STRSIZE];
	int ready, exec;
	pid_t pid;
};

int procCmp(const void *p1, const void *p2);
void unitsOfTime(unsigned T);
int getPolicyID(char *policy);
void getStartSeq(int* startSequence, struct procData* pList, int n);
pid_t startProcess(int exec);

int main() {
	char policy[STRSIZE];
	scanf("%s", policy);
	int policyID = getPolicyID(policy);
	
	int nProcesses; scanf("%d", &nProcesses);
	struct procData pList[nProcesses+1]; 
	for(int n = 1; n <= nProcesses; n++)
		scanf("%s %d %d", pList[n].name, &(pList[n].ready), &(pList[n].exec));
#ifdef inputDEBUG
	fprintf(stderr, "Policy: %s (ID %d)\n", policy, policyID);
	for(int n = 1; n <= nProcesses; n++)
		fprintf(stderr, "%s %d %d\n", pData[n].name, pData[n].ready, pData[n].exec);
#endif
	
	qsort(pList, nProcesses, sizeof(struct procData), procCmp);
#ifdef DEBUG
	for(int n = 1; n <= nProcesses; n++) {
		fprintf(stderr, "(%d, %d) ", id, pList[n].ready);
	}
#endif
	
	int t = 0;
	for(int n = 1; n <= nProcesses; n++) {
		struct procData current = pList[n];
		current.pid = startProcess(current.exec);
		unitsOfTime(current.ready - t);
		t = current.ready;
		printf("%s %d\n", current.name, current.pid);
	}
	
	for(int n = 1; n <= nProcesses; n++) {
		int pid = waitpid(-1, NULL, 0);
		fprintf(stderr, "Process ID %d terminated.\n", pid);
	}
}

// ==============================================================================

int procCmp(const void *p1, const void *p2) {
	return ((struct procData *)p1)->ready - ((struct procData *)p2)->ready;
}

void unitsOfTime(unsigned T) {
	while(T--) {
		volatile unsigned long i; for(i=0;i<1000000UL;i++);
	}
}

int getPolicyID(char *policy) {
	char policyNames[4][STRSIZE]={"FIFO", "RR", "SJF", "PSJF"};
	for(int id = 0; id < 4; id++)
		if(strcmp(policy, policyNames[id]) == 0)
			return id;
	return -1;
}

pid_t startProcess(int exec) {
	int pid;
	if((pid = fork()) == 0) {
		struct timespec start, end;
		clock_gettime(CLOCK_REALTIME, &start);
		unitsOfTime(exec);
		clock_gettime(CLOCK_REALTIME, &end);
		char msg2dmesg[STRSIZE];
		sprintf(msg2dmesg, "[project1] %d %lu.%lu %lu.%lu\n", getpid(),
		        start.tv_sec, start.tv_nsec, end.tv_sec, end.tv_nsec);
		syscall(PRINTK, msg2dmesg);
#ifdef DEBUG
		fprintf(stderr, "%s", msg2dmesg);
#endif
		exit(0);
	}
	else
		return pid;
}