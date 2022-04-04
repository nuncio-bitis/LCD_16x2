// ****************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <wiringPi.h>

#include "cpuUsage.h"

// ****************************************************************************

void read_pid_stats(struct cpuStats *st)
{
    FILE *fp;
    pid_t mypid = getpid();
    char procstat[64];
    snprintf(procstat, sizeof(procstat), "/proc/%d/stat", mypid);

    fp = fopen(procstat, "r");
    int pid, ppid, pgrp, session, tty_nr, tpgid;
    unsigned int flags;
    unsigned long minflt, cminflt, majflt, cmajflt, utime, stime;
    char cm[255], cpun[255], stat;
    if (fscanf(fp, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu",
               &pid, cm, &stat, &ppid, &pgrp, &session, &tty_nr, &tpgid, &flags,
               &minflt, &cminflt, &majflt, &cmajflt, &utime, &stime) == EOF)
    {
        perror("fscanf");
    }
    printf("pid: %d command: %s utime: %ld stime: %ld\n", pid, cm, utime, stime);
    fclose(fp);
}

// ****************************************************************************

/*
  /proc/stat, first line:
  cpu  31393 169 7639 353168 4164 0 57 0 0 0
    cpu - All CPUs combined
    user – time spent in user mode.
    nice – time spent in user mode with low priority.
    system – time spent in system mode.
    idle – time spent in the idle task.
    iowait –  time waiting for I/O to complete.
    irq – time servicing hardware interrupts.
    softirq – time servicing software interrupts.
    steal – time spent in other operating systems when running in a virtualized environment.
    guest – time spent running a virtual CPU for guest operating systems.
    guest_nice – time spent running a low priority virtual CPU for guest operating systems.

 */
void read_cpu_stats(struct cpuStats *st)
{
    char cpun[255];
    FILE *fp = fopen("/proc/stat", "r");

    st->capTime = millis();

    if (fscanf(fp, "%s %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld", cpun,
                &(st->user),
                &(st->nice),
                &(st->system),
                &(st->idle),
                &(st->iowait),
                &(st->irq),
                &(st->softirq),
                &(st->steal),
                &(st->guest),
                &(st->guest_nice) ) == EOF)
    {
        perror("fscanf");
    }

    st->activeTime =
        st->user +
        st->nice +
        st->system +
        st->irq +
        st->softirq +
        st->steal +
        st->guest +
        st->guest_nice;
    st->idleTime = st->idle + st->iowait;
    st->totalTime = st->activeTime + st->idleTime;

    //printf("  user: %ld\n", st->user);
    //printf("  nice: %ld\n", st->nice);
    //printf("  system: %ld\n", st->system);
    //printf("  idle: %ld\n", st->idle);
    //printf("  iowait: %ld\n", st->iowait);
    //printf("  irq: %ld\n", st->irq);
    //printf("  softirq: %ld\n", st->softirq);
    //printf("  steal: %ld\n", st->steal);
    //printf("  guest: %ld\n", st->guest);
    //printf("  guest_nice: %ld\n", st->guest_nice);

    fclose(fp);
}

// ****************************************************************************

void CpuUsage(struct cpuUsageStats *usg, struct cpuStats *st1, struct cpuStats *st2)
{
    unsigned long dActive = st2->activeTime - st1->activeTime;
    unsigned long dIdle = st2->idleTime - st1->idleTime;
    unsigned long dTotal = st2->totalTime - st1->totalTime;

    usg->deltaTime = st2->capTime - st1->capTime;

    usg->activePct = 100.0 * (double)dActive / (double)dTotal;
    usg->idlePct = 100.0 * (double)dIdle / (double)dTotal;
}

// ****************************************************************************

#ifdef MAIN
int main()
{
    size_t cnt = 0;
    struct cpuStats st1, st2;
    struct cpuUsageStats usg;

    memset(&st1, 0, sizeof(st1));
    memset(&st2, 0, sizeof(st2));
    memset(&usg, 0, sizeof(usg));

    while (cnt++ < 20)
    {
        //read_pid_stats(&st1);
        read_cpu_stats(&st1);
        sleep(1);
        //read_pid_stats(&st2);
        read_cpu_stats(&st2);
        CpuUsage(&usg, &st1, &st2);
        printf("cnt: %2d Active: %.2f%% Idle: %.2f%% ; %ld mS\n", cnt, usg.activePct, usg.idlePct, usg.deltaTime);
    }
}
#endif

// ****************************************************************************
