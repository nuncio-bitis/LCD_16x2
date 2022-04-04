//******************************************************************************
//
// FILE NAME: do_clock.c
//
// DESCRIPTION:
//   Display date and time on a 16x2 LCD display.
//
// AUTHOR: J. Parziale
//
//******************************************************************************

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>

#include "lcd_16x2.h"

// ****************************************************************************

static bool terminate = false;

// ****************************************************************************

void sighandler(int sig)
{
  fprintf(stderr, "\nSignal %d caught by %d\n", sig, getpid());
  terminate = true;
}

// ****************************************************************************

int main(int argc, char *argv[])
{
    char line1[16];
    char line2[16];

    // Catch Ctl-C, etc, to be able to terminate program
    signal(SIGABRT, &sighandler);
    signal(SIGTERM, &sighandler);
    signal(SIGINT,  &sighandler);

    signal(SIGHUP, SIG_IGN);

    lcdInit();

    // Return cursor to home position
    lcdCommand(CMD_RET_HOME);

    while (!terminate)
    {
        time_t now = time(NULL);
        struct tm * local_time = localtime(&now);

        // Line 1 format: Dow dd Mmm YYYY
        strftime(line1, sizeof(line1), "%a %d %b %Y", local_time);

        // Line 2 format: hh:mm:ss AM/PM EDT/EST
        strftime(line2, sizeof(line2), "%r %Z", local_time);

        (void)lcdText(line1, LCD_LINE1);
        (void)lcdText(line2, LCD_LINE2);

        usleep(100 * 1000);
    } // end while

    // Return cursor to home position
    lcdCommand(CMD_RET_HOME);

    lcdCommand(CMD_CLR_DISP);

    return EXIT_SUCCESS;
}

// ****************************************************************************
