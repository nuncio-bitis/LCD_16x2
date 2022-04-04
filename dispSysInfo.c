//******************************************************************************
//
// FILE NAME: dispSysInfo.c
//
// DESCRIPTION:
//   Display various system info on a 16x2 LCD display.
//
// AUTHOR: J. Parziale
//
//******************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

#include <wiringPi.h>

#include "lcd_16x2.h"

#include "cpuUsage.h"
#include "memUsage.h"

// ****************************************************************************

// @DEBUG
#define HERE() do { printf ("%s(%d).%s\r\n", __FILE__, __LINE__, __FUNCTION__); } while(0)
#define DPRINTF(fmt, args...) do { \
        printf ("%s(%d).%s ", __FILE__, __LINE__, __FUNCTION__); \
        printf(fmt, ## args); \
        fflush(stdout); \
} while (0)
// @DEBUG

// ****************************************************************************
// Definitions for buttons

// Define the GPIOs (BCM) that buttons are attached to.
// Button1, Button2, ... , ButtonN-1
static const int buttonGpios[] = { 26, 16, 20, 21 };

// Button flags: 1 = button was pressed.
// @NOTE The size of this array must match the number of GPIOs specified in buttonGpios above
static int buttons[4];

static const int nButtons = (sizeof (buttonGpios) / sizeof (int));

// Debounce time in mS
#define DEBOUNCE_TIME 50

// ****************************************************************************

typedef enum
{
    // These values match button indices
    CLOCK   = 0,
    WEATHER = 1,
    CPU     = 2,
    OTHER   = 3, // @TODO
    INVALID = 9  // Used to exit application
} Mode;
static Mode systemMode = CLOCK;

static char line1[16];
static char line2[16];

static bool terminate = false;

static void Signal_handler(int sig);
static void *debounce (UNUSED void *arg);

static void doClock(void);
static void doWeather(void);
static void doCPU(void);
static double getCPUPercent(void);
static double getMemPercent(void);

// ****************************************************************************

int main(int argc, char *argv[])
{
    // Catch Ctl-C, etc, to be able to terminate program
    signal(SIGABRT, &Signal_handler);
    signal(SIGTERM, &Signal_handler);
    signal(SIGINT,  &Signal_handler);

    signal(SIGHUP, SIG_IGN);

    // ----------------------------------------------------

    wiringPiSetupGpio(); // use BCM numbering

    // Set up the button debounce thread
    pthread_t debounceThreadId;
    pthread_create (&debounceThreadId, NULL, debounce, NULL) ;

    // Initialize the LCD display
    lcdInit();
    // Return cursor to home position
    lcdCommand(CMD_RET_HOME);

    // ----------------------------------------------------

    while (!terminate && (systemMode != INVALID))
    {
        switch(systemMode)
        {
        case CLOCK:
            doClock();
            break;

        case WEATHER:
            doWeather();
            break;

        case CPU:
            doCPU();
            break;

        // @TODO
        case OTHER:
            break;

        default:
            printf("Invalid mode; %d\n", systemMode);
            break;
        }

        // Clear screen when mode changes.
        memset(line1, ' ', sizeof(line2));
        memset(line2, ' ', sizeof(line2));
        lcdCommand(CMD_RET_HOME);
        lcdCommand(CMD_CLR_DISP);

        delayMs(250);
    } // end while

    // ----------------------------------------------------

    // Return cursor to home position & clear display
    lcdCommand(CMD_RET_HOME);
    lcdCommand(CMD_CLR_DISP);

    // Wait for debounce thread to finish.
    pthread_join(debounceThreadId, NULL);

    printf("%s done.\n", argv[0]);

    return EXIT_SUCCESS;
}

// ****************************************************************************

void Signal_handler(int sig)
{
    fprintf(stderr, "\nSignal %d caught by %d\n", sig, getpid());
    terminate = true;
    systemMode = INVALID;
}

//**********************************************************************************************************************

/* debounce
 *  Thread to monitor button GPIOs and debounce them.
 */
static void *debounce (UNUSED void *arg)
{
    int masks[nButtons];
    int button_times[nButtons];

    // Set the GPIOs as inputs
    // Also set up the gpio bank masks for each button
    for (int i = 0; i < nButtons; ++i)
    {
        pinMode (buttonGpios[i], INPUT);
        pullUpDnControl (buttonGpios[i], PUD_UP);

        // Set up GPIO masks for the GPIO bank
        masks[i] = (1 << buttonGpios[i]);

        // Clear button flags and counters
        buttons[i] = 0;
        button_times[i] = 0;
    }

    // --------------------------------
    // GPIO monitor & debounce loop
    uint32_t bank0 = 0;
    uint32_t prev_button_states[nButtons];
    uint32_t prev_buttons[nButtons];

    while (!terminate)
    {
        usleep(5 * 1000);

        bank0 = digitalReadBank(0);

        for (int i=0; i < nButtons; ++i)
        {
            int buttonState = (bank0 & masks[i]);

            // If button changed state, reset its timer.
            if (buttonState != prev_button_states[i])
            {
                button_times[i] = millis();
            }

            if ((millis() - button_times[i]) >= DEBOUNCE_TIME)
            {
                // Pressed button = LOW = 0
                // Set the state to 1 if pressed, 0 if released.
                buttons[i] = !buttonState;

                // Change system mode only if button went from not pressed to pressed.
                if (buttons[i] && (buttons[i] != prev_buttons[i]))
                {
                    systemMode = i;
                }
                prev_buttons[i] = buttons[i];
            }

            prev_button_states[i] = buttonState;
        }
    } // end while

    // --------------------------------

    printf("Debounce done.\n");

    return (void *)NULL;
}

// ****************************************************************************

void doClock(void)
{
    while (systemMode == CLOCK)
    {
        time_t now = time(NULL);
        struct tm * local_time = localtime(&now);

        // Line 1 format: Dow dd Mmm YYYY
        strftime(line1, sizeof(line1), "%a %d %b %Y", local_time);

        // Line 2 format: hh:mm:ss AM/PM EDT/EST
        strftime(line2, sizeof(line2), "%r %Z", local_time);

        (void)lcdText(line1, LCD_LINE1);
        (void)lcdText(line2, LCD_LINE2);

        delayMs(250);
    }
}

// ****************************************************************************

void doWeather(void)
{
    const int updatePeriod = (15 * 60 * 1000); // 15 min in milliseconds
    const int loopPeriod = 250; // mS - wake up this often to check systemMode
    int loopCount = updatePeriod; // So we immediately display info

    while (systemMode == WEATHER)
    {
        if (loopCount >= updatePeriod)
        {
            loopCount = 0;

            memset(line1, ' ', sizeof(line2));
            memset(line2, ' ', sizeof(line2));

            // Get output from weather python script
            FILE *fp;

            // Open the command for reading.
            fp = popen("/home/pi/bin/weather.py -d", "r");
            if (fp == NULL)
            {
                printf("Failed to run command\n" );
            }
            else
            {
                char info[20];

                // Read the output
                fgets(info, sizeof(info), fp);
                strncpy(line1, info, sizeof(line1));

                fgets(info, sizeof(info), fp);
                strncpy(line2, info, sizeof(line2));

                pclose(fp);

                (void)lcdText(line1, LCD_LINE1);
                (void)lcdText(line2, LCD_LINE2);
            }

        } // end if(loopCount)

        delayMs(loopPeriod);
        loopCount += loopPeriod;
//        delayMs(1000); // @DEBUG
//        loopCount = updatePeriod; // @DEBUG
    } // end while(systemMode)
}

// ****************************************************************************

void doCPU(void)
{
    // |++++++++++++++++|
    // |C:###___________| CPU usage %
    // |M:######________| Mem avail %
    // |++++++++++++++++|
    // There are 14 characters being dedicated to show the percentage.
    // Percentage per bar = 100% / 14
    // n_bars = (int)(percentage / (100/14))
    //        = (int)(14 * percentage / 100)
    const int n_chars = 14;

    while (systemMode == CPU)
    {
        double cpu_percent = getCPUPercent();
        double mem_avail = getMemPercent();

#if 0
        int cpu_bars = (int)(n_chars * cpu_percent / 100.0 + 0.5);
        int mem_bars = (int)(n_chars * mem_avail / 100.0 + 0.5);

        memset(line1, '_', sizeof(line1));
        line1[0] = 'C'; line1[1] = ':';
        for (int i=0; (i < cpu_bars) && (i <= 14); ++i)
        {
            line1[i+2] = '#';
        }

        memset(line2, '_', sizeof(line2));
        line2[0] = 'M'; line2[1] = ':';
        for (int i=0; (i < mem_bars) && (i <= 14); ++i)
        {
            line2[i+2] = '#';
        }
#else
        // Line 1: C:xx.x% M:xx.x%
        sprintf(line1, "C:%4.1f%% M:%4.1f%%", cpu_percent, mem_avail);

        // Line 2: hh:mm:ss AM/PM EDT/EST
        time_t now = time(NULL);
        struct tm * local_time = localtime(&now);
        strftime(line2, sizeof(line2), "%r %Z", local_time);
#endif

        (void)lcdText(line1, LCD_LINE1);
        (void)lcdText(line2, LCD_LINE2);

        delayMs(1000);
    } // end while(systemMode)
}

double getCPUPercent(void)
{
    static bool first = true;
    static struct cpuStats st1, st2;

    double ret = 0.0;
    struct cpuUsageStats usg;

    read_cpu_stats(&st2);

    if (first)
    {
        first = false;
    }
    else
    {
        CpuUsage(&usg, &st1, &st2);
        //printf("CPU: Active %.2f%% Idle: %.2f%% ; %ld mS\n", usg.activePct, usg.idlePct, usg.deltaTime); // @DEBUG
        ret = usg.activePct;
    }
    memcpy(&st1, &st2, sizeof(st2));

    return ret;
}

double getMemPercent(void)
{
    struct memStats mst;
    getMemStats(&mst);
    double freePct = 100.0 * (double)mst.free / (double)mst.total;
    //printf("Memory: Total %ld  Free %ld (%.2f%%)\n", mst.total, mst.free, freePct); // @DEBUG
    return freePct;
}

// ****************************************************************************
