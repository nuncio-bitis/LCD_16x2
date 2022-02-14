//******************************************************************************
//
// FILE NAME: testLcd.c
//
// DESCRIPTION:
//   Send text to a 16x2 LCD display.
//   To build: gcc testLcd.c -o testLcd -lwiringPi
//
// AUTHOR: J. Parziale
//
//******************************************************************************

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

#include "lcd.h"

// ****************************************************************************

int main(int argc, char *argv[])
{
    lcdInit();

    // Return cursor to home position
    lcdCommand(CMD_RET_HOME);

    if (argc > 1)
    {
        (void)lcdText(argv[1], LCD_LINE1);
        if (argc > 2)
        {
            (void)lcdText(argv[2], LCD_LINE2);
        }
    }
    else
    {
        (void)lcdText("  LCD DISPLAY", LCD_LINE1);
        (void)lcdText(" Hello World!!!", LCD_LINE2);
    }

    // Return cursor to home position
    lcdCommand(CMD_RET_HOME);

    return EXIT_SUCCESS;
}

// ****************************************************************************
