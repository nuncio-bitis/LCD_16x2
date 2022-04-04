//******************************************************************************
//
// FILE NAME: memUsage.h
//
// DESCRIPTION:
//   Routines for getting memory stats.
//
// AUTHOR: J. Parziale
//
//******************************************************************************

#ifndef _MEMUSAGE_H
#define _MEMUSAGE_H
//******************************************************************************

#include <stdint.h>

struct memStats
{
    unsigned long total;
    unsigned long free;
};

extern void getMemStats(struct memStats *mst);

//******************************************************************************
#endif // _MEMUSAGE_H
