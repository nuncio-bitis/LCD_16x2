//******************************************************************************
//
// FILE NAME: lcd_text.c
//
// DESCRIPTION:
//   Send text to a 16x2 LCD display.
//   Format: lcd_text <line 1 text> [<line 2 text>]
//
// AUTHOR: J. Parziale
//
//******************************************************************************

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

#include "lcd_16x2.h"

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
        // No parameters - display some test text.
        (void)lcdText("[++ABCDEFGHIJ++]", LCD_LINE1);
        (void)lcdText("[++1234567890++]", LCD_LINE2);
    }

    // Return cursor to home position
    lcdCommand(CMD_RET_HOME);

    return EXIT_SUCCESS;
}

// ****************************************************************************
