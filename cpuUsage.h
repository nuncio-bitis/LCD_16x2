//******************************************************************************
//
// FILE NAME: cpuUsage.h
//
// DESCRIPTION:
//   Routines for getting CPU and process stats.
//
// AUTHOR: J. Parziale
//
//******************************************************************************

#ifndef _CPUUSAGE_H
#define _CPUUSAGE_H
//******************************************************************************

#include <stdint.h>

struct cpuStats
{
    unsigned long capTime;
    unsigned long activeTime;
    unsigned long idleTime;
    unsigned long totalTime;

    unsigned long user;
    unsigned long nice;
    unsigned long system;
    unsigned long idle;
    unsigned long iowait;
    unsigned long irq;
    unsigned long softirq;
    unsigned long steal;
    unsigned long guest;
    unsigned long guest_nice;
};

struct cpuUsageStats
{
    double activePct;
    double idlePct;
    unsigned long deltaTime;
};

extern void read_pid_stats(struct cpuStats *st);
extern void read_cpu_stats(struct cpuStats *st);
extern void CpuUsage(struct cpuUsageStats *usg, struct cpuStats *st1, struct cpuStats *st2);

//******************************************************************************
#endif // _CPUUSAGE_H