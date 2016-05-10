//-------------------------------------------------------------------
// tstat.h
//
// Created for "Arduino in a Nutshell", 2015, J. M. Hughes
// Chapter 12
//-------------------------------------------------------------------

#ifndef TSTAT_H
#define TSTAT_H

#define ROTENC_A    0
#define ROTENC_B    1
#define ROTENC_SW   2

#define LCD_D4      4       // Predefined by the LCD shield
#define LCD_D5      5       //
#define LCD_D6      6       //
#define LCD_D7      7       //
#define LCD_RS      8       //
#define LCD_E       9       //
#define LCD_LED     10      //
#define LCD_SW      A0      //

#define RTC_SCLK    11
#define RTC_IO      12
#define RTC_CE      13

#define RY1         15      // A1
#define RY2         16      // A2
#define RY3         17      // A3
#define RY4         18      // A4

#define DHT1        3       // Internal DHT22
#define DHT2        19      // A5, external DHT22

#define SWLONG      1000    // ms
#define SWSHORT     500     // ms

#define LCDLIGHT    15000   // On time for LCD backlight

#define PROF_MAX    4       // Max # of profiles
#define PROF_OFF    0       // No active profile
#define PROF_DAY    1       // Day profile
#define PROF_NITE   2       // Night profile
#define PROF_WKND   3       // Weekend profile

#define MODE_MAX    5       // Max # of modes
#define MODE_OFF    0       // System off
#define MODE_COOL   1       // Cool only
#define MODE_HEAT   2       // Heat only
#define MODE_FAN    3       // Fan only
#define MODE_AUTO   4       // Automatic mode

#define SCALE_F     0       // Temp scale in Farenheit
#define SCALE_C     1       // Temp scale in Centigrade

#define MAX_DISP_TIME 2000  // 2 sec screen toggle time

typedef struct {
    int op_mode;
    int start_hr;
    int start_min;
    int duration;
    int tset;
    int hystrange;
} prof_params;

#endif
