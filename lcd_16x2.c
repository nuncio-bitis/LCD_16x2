//******************************************************************************
//
// FILE NAME: lcd_16x2.c
//
// DESCRIPTION:
//   Interface for writing text to a 16x2 LCD display.
//
// AUTHOR: J. Parziale
//
//******************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <unistd.h>

#include "lcd_16x2.h"

// ****************************************************************************

// This can be set by making the 'debug' target (make debug)
//#define DEBUG

#ifdef DEBUG
  #define DBG_PRINT(msg, ...) printf("%s(%d) %s | " msg "\n", __FILE__, __LINE__, __func__, __VA_ARGS__)
#else
  #define DBG_PRINT(msg, ...)
#endif

// ****************************************************************************

void SetCmdMode()
{
    DBG_PRINT("LCD_RS=0", NULL);
    digitalWrite(LCD_RS, 0); // set for commands
    usleep(500);
}

void SetChrMode()
{
    DBG_PRINT("LCD_RS=1", NULL);
    digitalWrite(LCD_RS, 1); // set for characters
    usleep(500);
}

void pulseEnable()
{
    digitalWrite(LCD_E, HIGH);
    usleep(50); // min 40 uS is needed for commands

    digitalWrite(LCD_E, LOW);
    usleep(50); // min 40 uS is needed for commands
}

// ****************************************************************************

// For (undocumented) initialization
void lcd4bits(char bits)
{
    DBG_PRINT("0x%02X", bits);

    digitalWrite(LCD_D4, (bits & 0x10));
    digitalWrite(LCD_D5, (bits & 0x20));
    digitalWrite(LCD_D6, (bits & 0x40));
    digitalWrite(LCD_D7, (bits & 0x80));
    pulseEnable();
}

/*
  send a byte to the lcd in two nibbles
  before calling use SetChrMode or SetCmdMode to determine whether to send character or command
*/
void lcdByte(char bits)
{
    DBG_PRINT("0x%02X", bits);

    digitalWrite(LCD_D4, (bits & 0x10));
    digitalWrite(LCD_D5, (bits & 0x20));
    digitalWrite(LCD_D6, (bits & 0x40));
    digitalWrite(LCD_D7, (bits & 0x80));
    pulseEnable();

    digitalWrite(LCD_D4, (bits & 0x1));
    digitalWrite(LCD_D5, (bits & 0x2));
    digitalWrite(LCD_D6, (bits & 0x4));
    digitalWrite(LCD_D7, (bits & 0x8));
    pulseEnable();
}

void lcdCommand(uint8_t cmd)
{
    SetCmdMode();
    lcdByte(cmd);

    if ((CMD_CLR_DISP == cmd) || (CMD_RET_HOME == cmd))
    {
        usleep(2 * 1000); // min 1.53 ms required after a screen clear or return-to-home
    }
}

// ****************************************************************************

/*
 * Write text to the specified display line
 * Returns # characters written, or -1 on error.
 */
int lcdText(const char *s, enum LcdLine line)
{
    // Set the AC to the specified line.
    uint8_t cmd = CMD_SET_DDRAM_ADDR;
    if (line == LCD_LINE1)
    {
        cmd += LINE1_ADDR;
    }
    else if (line == LCD_LINE2)
    {
        cmd += LINE2_ADDR;
    }
    else
    {
        return -1;
    }
    lcdCommand(cmd);

    // @TODO If text length > 16 characters, set display shift

    // Write the text
    SetChrMode();
    int i = 0;
    DBG_PRINT("LCD line %d", line+1);
    while (*s)
    {
        DBG_PRINT("%c", *s);
        lcdByte(*s);
        s++;
        i++;
    }

    return i;
}

// ****************************************************************************

void lcdInit(void)
{
    DBG_PRINT("Initializing...", 0);

    wiringPiSetupGpio(); // use BCM numbering

    // set up pi pins for output
    digitalWrite(LCD_RS, 0); pinMode(LCD_RS, OUTPUT);
    digitalWrite(LCD_E,  0); pinMode(LCD_E,  OUTPUT);
    digitalWrite(LCD_D4, 0); pinMode(LCD_D4, OUTPUT);
    digitalWrite(LCD_D5, 0); pinMode(LCD_D5, OUTPUT);
    digitalWrite(LCD_D6, 0); pinMode(LCD_D6, OUTPUT);
    digitalWrite(LCD_D7, 0); pinMode(LCD_D7, OUTPUT);

    // initialise LCD
    SetCmdMode();   // set for commands

    // The Function Set command needs to be sent at least 3x
    // (discovered by experimentation)
    // This is horribly undocumented.
    lcd4bits(CMD_FUNCTION_SET + LCD_DATA_LENGTH_8);
    lcd4bits(CMD_FUNCTION_SET + LCD_DATA_LENGTH_4);
    lcd4bits(CMD_FUNCTION_SET + LCD_DATA_LENGTH_4);

    lcdCommand(CMD_FUNCTION_SET + LCD_DATA_LENGTH_4 + LCD_2_LINES + LCD_FONT_5x8); usleep(35 * 1000);
    lcdCommand(CMD_DISPLAY_ON_OFF_CTL + LCD_DISPLAY_ON + LCD_CURSOR_OFF + LCD_BLINK_OFF);
    lcdCommand(CMD_CURSOR_DISP_SHIFT + LCD_SHIFT_CURSOR + LCD_SHIFT_LEFT);
    lcdCommand(CMD_ENTRY_MODE_SET + LCD_INCREMENT + LCD_DISPLAY_SHIFT_OFF);
    lcdCommand(CMD_CLR_DISP);

    usleep(2 * 1000); // min 1.53 ms required for a screen clear
}

// ****************************************************************************
