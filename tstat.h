//-------------------------------------------------------------------
// tstat.h
//
// Created for "Arduino: A Technical Reference", 2015, J. M. Hughes
// Chapter 12
//-------------------------------------------------------------------

#ifndef TSTAT_H
#define TSTAT_H

#define ROT_A       0       // Rotary encoder interface
#define ROT_B       1       //
#define ROT_SW      2       //
#define ROT_STEP    4       // Enc outputs 4 counts per step

#define LCD_D4      4       // LCD shield data and control
#define LCD_D5      5       //
#define LCD_D6      6       //
#define LCD_D7      7       //
#define LCD_RS      8       //
#define LCD_E       9       //
#define LCD_LED     10      //
#define LCD_SW      A0      // Not used

#define RTC_SCLK    11      // RTC serial clock
#define RTC_IO      12      // RTC data
#define RTC_CE      13      // RTC enable

#define RY1         15      // A1 used as dig output
#define RY2         16      // A2 used as dig output
#define RY3         17      // A3 used as dig output
#define RY4         18      // A4 used as dig output

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

#define SHOW_ALL    0       // Show all of disp screen
#define SHOW_SUB    1       // Show sub-set of screen
#define SHOW_PSET   0       // Show predefined profile data
#define SHOW_PGET   1       // Show edited profile data

#define MODE_MAX    5       // Max # of modes
#define MODE_OFF    0       // System off
#define MODE_COOL   1       // Cool only
#define MODE_HEAT   2       // Heat only
#define MODE_FAN    3       // Fan only
#define MODE_AUTO   4       // Automatic mode

#define SCALE_F     0       // Temp scale in Farenheit
#define SCALE_C     1       // Temp scale in Centigrade

#define MAX_DISP_TIME 2000  // 2 sec screen toggle time
#define MAX_ILUM_TIME 30000 // Turn off display after 30s idle time

// EEPROM Layout
// The double type gets 4 bytes of storage. With the ATMega386 boards
// it's the same as a float.
#define EE_USED     0   // 1 byte       Assume zero if EEPROM not yet used
#define EE_TSET     1   // 4 bytes      gv_temp_set (as 4-byte float)
#define EE_CSET     5   // 4 bytes      gv_hyst_set (as 4-byte float)
#define EE_OSET     9   // 1 byte       gv_over_time
#define EE_MSET     10  // 1 byte       gv_mode_set
#define EE_PSET     11  // 1 byte       gv_prof_set
#define EE_TSCL     12  // 1 byte       gv_curr_scale
// profile structure elements are each 20 bytes long
#define EE_PDATA0   13  // PROF_OFF     gv_profile[0]
#define EE_PDATA1   25  // PROF_DAY     gv_profile[1]
#define EE_PDATA2   37  // PROF_NITE    gv_profile[2]
#define EE_PDATA3   49  // PROF_WKND    gv_profile[3]

// Used to detect button presses on the LCD module
// This is used for diagnostics and error recovery only
#define NUM_LCD_BUTTONS     5
#define LCD_BTN_NONE        -1
#define LCD_BTN_RIGHT       0
#define LCD_BTN_UP          1
#define LCD_BTN_DOWN        2
#define LCD_BTN_LEFT        3
#define LCD_BTN_SELECT      4

// Default values
#define DEF_TEMP_SET        22.0
#define DEF_HYST_SET        3.0
#define DEF_OVERTM          60
#define DEF_MODE            0
#define DEF_PROF            0


// Profile settings storage
// 12 bytes per element in profile array
typedef struct {
    uint8_t op_mode;
    uint8_t start_hr;
    uint8_t start_min;
    uint8_t duration;
    double  tset;
    double  hystrange;
} prof_params;


enum DSPSTATE {
    DS0,
    DS1,
    DS2,
    DS3,
    DS4
};

enum SC3NP {
    SC3NPNULL = 0,
    SC3NPTEMP,
    SC3NPCYCL,
    SC3NPTSCL,
    SC3NPOVRT,
    SC3NPMODE,
    SC3NPPROF,
    SC3NPSET
};

enum SC3PR {
    SC3PRNULL = 0,
    SC3PRTSCL,
    SC3PRPROF
};

enum SC4ST {
    SC4NULL = 0,
    SC4PROF,
    SC4MODE,
    SC4BTIME,
    SC4ETIME,
    SC4TEMP,
    SC4CSET,
    SC4SAVE
};

enum SC5ST {
    SC5NULL = 0,
    SC5YEAR,
    SC5MON,
    SC5DAY,
    SC5HOUR,
    SC5MIN,
    SC5SET,
    SC5DOW
};

#endif
