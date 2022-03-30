//******************************************************************************
//
// FILE NAME: lcd.h
//
// DESCRIPTION:
//   Interface for writing text to a 16x2 LCD display.
//
// AUTHOR: J. Parziale
//
//******************************************************************************

#ifndef _LCD_H
#define _LCD_H
//******************************************************************************

#include <stdint.h>

// ****************************************************************************
// GPIO setup

#define RPI4 0
#define RPI5 1
#define MACHINE RPI5

// Define GPIO pins (NOTE: this assumes 4-bit mode is being used)
#if MACHINE == RPI5
#define LCD_RS 18   // pin 4
#define LCD_E  23   // pin 6
#define LCD_D4 24   // pin 11
#define LCD_D5 25   // pin 12
#define LCD_D6 8    // pin 13
#define LCD_D7 7    // pin 14
#elif MACHINE == RPI4
#define LCD_RS 26   // pin 4
#define LCD_E  19   // pin 6
#define LCD_D4 13   // pin 11
#define LCD_D5 6    // pin 12
#define LCD_D6 5    // pin 13
#define LCD_D7 21   // pin 14
#else
#error INCORRECT MACHINE SPECIFICATION
#endif

//******************************************************************************
// LCD hardware definitions
// LCD specs: https://panda-bg.com/datasheet/2134-091834-LCD-module-TC1602D-02WA0-16x2-STN.pdf

// LCD Commands
#define CMD_CLR_DISP 0x01
#define CMD_RET_HOME 0x02
#define CMD_ENTRY_MODE_SET 0x04
    // Bits 1:0 = I/D, SH
    // I/D = 1 : increment, 0 : decrement
    // SH = 1 : display shift on, 0 : display shift off
#define CMD_DISPLAY_ON_OFF_CTL 0x08
    // Bits 2:0 = D, C, B
    // D=1: Display ON
    // C=1: Cursor ON
    // B=1: Cursor blink ON
#define CMD_CURSOR_DISP_SHIFT 0x10
    // Bits 3:2 = S/C, R/L
    // S/C = 0 : shift cursor, 1 : shift display
    // R/L = 0 : shift left, 1 : shift right
#define CMD_FUNCTION_SET 0x20
    // Bits 4:2 = DL, N, F
    // DL = 1 : 8-bit mode, 0 : 4-bit mode
    // N = 1 : 2-line mode, 0 : 1-line mode
    // F = 1 : 5x11 dot font, 0 : 5x8 dots
#define CMD_SET_CGRAM_ADDR 0x40 // Bits 5:0 = AC5:AC0 (address counter)
#define CMD_SET_DDRAM_ADDR 0x80 // Bits 6:0 = AC6:AC0 (address counter)

// Entry Mode command
// I/D
#define LCD_DECREMENT 0x00
#define LCD_INCREMENT 0x02
// SH
#define LCD_DISPLAY_SHIFT_OFF 0x00
#define LCD_DISPLAY_SHIFT_ON  0x01

// Display ON/OFF control command
// D - display control
#define LCD_DISPLAY_OFF 0x00
#define LCD_DISPLAY_ON  0x04
// C - cursor control
#define LCD_CURSOR_OFF 0x00
#define LCD_CURSOR_ON  0x02
// B - cursor blink
#define LCD_BLINK_OFF 0x00
#define LCD_BLINK_ON  0x01

// Cursor or Display shift command
// S/C - Shift all display / shift cursor
#define LCD_SHIFT_CURSOR  0x00
#define LCD_SHIFT_DISPLAY 0x80
// R/L
#define LCD_SHIFT_LEFT  0x00
#define LCD_SHIFT_RIGHT 0x04

// Function Set command
// DL - data length
#define LCD_DATA_LENGTH_4 0x00
#define LCD_DATA_LENGTH_8 0x10
// N - number of lines displayed
#define LCD_1_LINE  0x00
#define LCD_2_LINES 0x08
// F - font
#define LCD_FONT_5x8  0x00
#define LCD_FONT_5x11 0x04

// DDRAM addresses of the first character of each line.
#define LINE1_ADDR 0x00
#define LINE2_ADDR 0x40

// ****************************************************************************

// Specification for defining which line to put text on.
enum LcdLine {
    LCD_LINE1 = 0,
    LCD_LINE2 = 1
};

// ****************************************************************************

// Public API functions.
void lcdInit(void);
void lcdByte(char bits);
void lcdCommand(uint8_t cmd);
int  lcdText(const char *s, enum LcdLine line);

// Internal functions.
// Users of this API should not need to call these.
void SetCmdMode();
void SetChrMode();
void pulseEnable();

//******************************************************************************
#endif // _LCD_H
