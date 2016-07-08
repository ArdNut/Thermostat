//-------------------------------------------------------------------
// tstat_gv.cpp
//
// Created for "Arduino: A Technical Reference", 2015, J. M. Hughes
// Chapter 12
//-------------------------------------------------------------------

#include "Arduino.h"
#include <LiquidCrystal.h>
#include "DS1302RTC.h"
#include "tstat.h"

// initialize the LCD and RTC objects
LiquidCrystal *lcd = new LiquidCrystal(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
DS1302RTC *rtc = new DS1302RTC(13, 12, 11);

bool  gv_rot_active = false;    // Encoder turned or pressed
bool  gv_long_sw    = false;    // Long switch action detected
bool  gv_short_sw   = false;    // Short switch action detected
bool  gv_rot_pos    = false;    // CW rotation
bool  gv_rot_neg    = false;    // CCW rotation

// The millisecond counter will roll over after about 50 days
// so the code will need to account for that to prevent the LED
// backlight from becoming "hung" in the on state.
unsigned long lcd_led_strt = 0;     // LED backlight start time
unsigned long disptime = 0;         // Screens 1/2 display timer

int   gv_year_val = 2016;       // year
char  gv_mon_val[4] = {"NUL"};  // month name
int   gv_day_val = 0;           // day
int   gv_curr_time = 0;         // current time (24 hour format)

bool  gv_input_active=false;

int   gv_curr_screen = 0;       // current active display screen

int   gv_temp_set = 0;             // Temperature set point
int   gv_hyst_set = 0;             // Hysteresis range

int   gv_mode_set = 0;             // Operation mode
int   gv_prof_set = 0;             // Profile ID

int   gv_tint_curr = 0.0;          // Interior temperature
int   gv_tint_last = 0.0;          // Interior temperature

int   gv_hint_curr = 0;            // Interior humidity
int   gv_hint_last = 0;            // Interior humidity

int   gv_text_curr = 0.0;          // External temperature
int   gv_text_last = 0.0;          // External temperature

int   gv_hext_curr = 0;            // External humidity
int   gv_hext_last = 0;            // External humidity

int   gv_curr_scale = SCALE_F;     // Temp display scale

prof_params gv_profile[PROF_MAX];  // Profiles

