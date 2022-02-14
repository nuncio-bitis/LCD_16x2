#################################################################################
#
# Makefile for LCD library and test application
#
# J. Parziale
#
#################################################################################
#
# This file uses wiringPi (Wiring Compatable library for the Raspberry Pi)
#
#################################################################################


#DEBUG += -g -O0
DEBUG += -O3
CC  = gcc
CPP = g++
INCLUDES = -I/usr/local/include -I ../wiringPi/wiringPi
CFLAGS  = $(DEBUG) $(INCLUDES) -Winline -pipe

LDFLAGS	= -L/usr/local/lib
LDLIBS    = -lwiringPi -lwiringPiDev -lpthread -lm

###############################################################################

BINS = testLcd

SRCS = \
	lcd.c \
	testLcd.c

HDRS = lcd.h

OBJS = $(SRCS:.c=.o)

###############################################################################

all: $(BINS)
	@echo ""

# Making this target turns on debugging statements in lcd.c
debug:
	$(MAKE) DEBUG=-DDEBUG all

testLcd: testLcd.c lcd.o
	@echo "----------------------------------------"
	@echo [link]
	@echo $(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)
	@echo

###############################################################################

.c.o:
	@echo "----------------------------------------"
	@echo [CC] $<
	@echo $(CC) -c $(CFLAGS) $< -o $@
	@$(CC) -c $(CFLAGS) $< -o $@
	@echo

.cpp.o:
	@echo "----------------------------------------"
	@echo [CC] $<
	@echo $(CPP) -c $(CFLAGS) $< -o $@
	@$(CPP) -c $(CFLAGS) $< -o $@
	@echo

clean:
	@echo "----------------------------------------"
	@echo "[Clean]"
	@rm -vf $(OBJS) $(BINS)
	@echo

info:
	@echo "----------------------------------------"
	@echo "[Info]"
	@echo DEBUG = $(DEBUG)
	@echo INCLUDES = $(INCLUDES)
	@echo LDFLAGS = $(LDFLAGS)
	@echo LDLIBS = $(LDLIBS)
	@echo CFLAGS = $(CFLAGS)
	@echo
	@echo HDRS = $(HDRS)
	@echo SRCS = $(SRCS)
	@echo OBJS = $(OBJS)
	@echo BINS = $(BINS)
	@echo

help:
	@echo "----------------------------------------"
	@echo "[Help]"
	@echo "make [all]    - will build all targets"
	@echo "   All targets: $(BINS)"
	@echo "make testLcd  - will build the LCD test application"
	@echo "make clean    - will delete the binaries and object files for all applications"
	@echo

###############################################################################
