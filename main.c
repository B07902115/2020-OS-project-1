#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <time.h>
#include <string.h>
#include <sched.h>

/* Constants */
#define STRSIZE 256
#define FIFO 0
#define RR 1
#define SJF 2
#define PSJF 3
#define T_QUANTUM 500
#define PRINTK 333
#define GETNSTIMEOFDAY 334

/* Global Variables */
int tUnits;
int nFinished;
int runningProc;
int lastSwitch;

struct procData {
	char name[STRSIZE];
	int tReady, tExec;
	pid_t pid;
};

struct queue {
   int data;
   struct queue *next;
};
struct queue *first = NULL, *end = NULL;
void queuePush(int id);
int queuePop();
int queueTop();

void unitsOfTime(unsigned T);
int getPolicyID(char *policy);

int procCmp(const void *p1, const void *p2);
pid_t execProc(int tExec);
int wakeProc(int pid);
int blockProc(int pid);
int scheduler(int nProc, struct procData *pList, int policyID);
int selectNext(int nProc, struct procData *pList, int policyID);

int main() {
	char policy[STRSIZE];
	scanf("%s", policy);
	int policyID = getPolicyID(policy);
	
	int nProc; scanf("%d", &nProc);
	struct procData pList[nProc+1]; 
	for(int n = 1; n <= nProc; n++)
		scanf("%s %d %d", pList[n].name, &(pList[n].tReady), &(pList[n].tExec));
	qsort(pList+1, nProc, sizeof(struct procData), procCmp);
	
	scheduler(nProc, pList, policyID);
	
#ifdef INPUTDEBUG
	fprintf(stderr, "Policy: %s (ID %d)\n", policy, policyID);
	for(int n = 1; n <= nProc; n++)
		fprintf(stderr, "%s %d %d\n", pList[n].name, pList[n].tReady, pList[n].tExec);
#endif
	
#ifdef QSORTDEBUG
	for(int n = 1; n <= nProc; n++) {
		fprintf(stderr, "(%d, %d) ", n, pList[n].tReady);
	}
	fprintf(stderr, "\n");
#endif
}

// ==============================================================================

void queuePush(int n)
{
   struct queue *tmp;
   tmp = (struct queue*)malloc(sizeof(struct queue));
   tmp->data = n;
   tmp->next = NULL;

   if (end == NULL)
      first = end = tmp;
   else {
      end->next = tmp;
      end = tmp;
   }
}

int queuePop()
{
   struct queue *tmp = first;

   if (tmp != NULL) {
      first = first->next;
      int val = tmp->data;
      free(tmp);
      return val;
   }
}

int queueTop()
{
   if (first != NULL)
	   return first->data;
   return -1;
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

int procCmp(const void *p1, const void *p2) {
	return ((struct procData *)p1)->tReady - ((struct procData *)p2)->tReady;
}

pid_t execProc(int tExec) {
	int pid;
	if((pid = fork()) == 0) {
		struct timespec start, end;
		syscall(GETNSTIMEOFDAY, &start);
		
		unitsOfTime(tExec);
		
		syscall(GETNSTIMEOFDAY, &end);
		syscall(PRINTK, getpid(), start.tv_sec, start.tv_nsec,
                                           end.tv_sec, end.tv_nsec);
#ifdef OUTPUTDEBUG
		fprintf(stderr, "%s", msg2dmesg);
#endif
		exit(0);
	}
	else if(pid < 0) {
		perror("Error returned by fork()");
		return -1;
	}
	return pid;
}

int wakeProc(int pid) {
	struct sched_param param;
	param.sched_priority = 0;
	int ret = sched_setscheduler(pid, SCHED_OTHER, &param);
	if(ret < 0) {
		perror("Error returned by sched_setscheduler()");
		return -1;
	}
	return ret;
}

int blockProc(int pid) {
	struct sched_param param;
	param.sched_priority = 0;
	int ret = sched_setscheduler(pid, SCHED_IDLE, &param);
	if(ret < 0) {
		perror("Error returned by sched_setscheduler");
		return -1;
	}
	return ret;
}

int scheduler(int nProc, struct procData *pList, int policyID) {
	for(int n = 1; n <= nProc; n++)
		pList[n].pid = -1;
	
	wakeProc(getpid());
	tUnits = 0; nFinished = 0; runningProc = -1;
	
	while(1) {
		if(runningProc != -1 && pList[runningProc].tExec == 0) {
			waitpid(pList[runningProc].pid, NULL, 0);
			runningProc = -1;
			nFinished++;
			
			if(nFinished == nProc)
				break;
		}
		
		for(int n = 1; n <= nProc; n++) {
			if(tUnits == pList[n].tReady) {
				pList[n].pid = execProc(pList[n].tExec);
				blockProc(pList[n].pid);
				if(policyID == RR)
					queuePush(n);
				printf("%s %d\n", pList[n].name, pList[n].pid);
				fflush(stdout);
			}
		}
		
	
		int nextProc = selectNext(nProc, pList, policyID);
		if(nextProc != -1) {
			if(nextProc != runningProc) {
#ifdef SCHEDDEBUG
				printf("%d running: %d next %d\n", tUnits, runningProc, nextProc);
#endif
				if(runningProc != -1) {
					blockProc(pList[runningProc].pid);
					if(policyID == RR)
						queuePush(runningProc);
				}
				wakeProc(pList[nextProc].pid);
				runningProc = nextProc;
				lastSwitch = tUnits;
			}
		}

#ifdef SCHEDDEBUG	
		if(tUnits % 100 == 0)
			printf("t = %d\n", tUnits);
#endif
	
	unitsOfTime(1);
		if(runningProc != -1)
			pList[runningProc].tExec--;
		tUnits++;
	}
	
	return 0;
}

int selectNext(int nProc, struct procData *pList, int policyID) {
	if(runningProc != -1 && (policyID == FIFO || policyID == SJF))
		return runningProc;
	
	int nextProc = -1;
	if(policyID == RR)
		nextProc = queuePop();
	else if(policyID == FIFO) {
		for(int n = 1; n <= nProc; n++) {
			if(pList[n].pid == -1 || pList[n].tExec == 0)
				continue;
			else if(nextProc == -1 || pList[n].tReady < pList[nextProc].tReady)
				nextProc = n;
		}
	}
	else if(policyID == PSJF || policyID == SJF) {
		for(int n = 1; n <= nProc; n++) {
			if(pList[n].pid == -1 || pList[n].tExec == 0)
				continue;
			else if(nextProc == -1 || pList[n].tExec < pList[nextProc].tExec)
				nextProc = n;
		}
	}
	return nextProc;
}