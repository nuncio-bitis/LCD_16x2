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

#include "memUsage.h"

// ****************************************************************************

void getMemStats(struct memStats *mst)
{
    char str1[64];
    char str2[64];
    char str3[64];
    char str4[64];
    FILE *fp = fopen("/proc/meminfo", "r");

    // First line = total memory
    if (fscanf(fp, "%s %ld %s", str1, &(mst->total), str2) == EOF)
    {
        perror("fscanf");
    }
    // Second line = free memory
    if (fscanf(fp, "%s %ld %s", str1, &(mst->free), str2) == EOF)
    {
        perror("fscanf");
    }

    fclose(fp);
}

// ****************************************************************************

#ifdef MAIN
int main(int argc, char *argv[])
{
    size_t cnt = 0;
    struct memStats mst;

    memset(&mst, 0, sizeof(mst));

    printf("%s\n", argv[0]);
    while (cnt++ < 20)
    {
        getMemStats(&mst);

        double freePct = 100.0 * (double)mst.free / (double)mst.total;

        printf("cnt: %2d Total: %ld  Free: %ld (%.2f%%)\n", cnt, mst.total, mst.free, freePct);

        sleep(1);
    }
}
#endif

// ****************************************************************************
