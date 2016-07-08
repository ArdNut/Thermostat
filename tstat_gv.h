//-------------------------------------------------------------------
// tstat_gv.h
//
// Created for "Arduino: A Technical Reference", 2015, J. M. Hughes
// Chapter 12
//-------------------------------------------------------------------

#ifndef TSTATGV_H
#define TSTATGV_H

#include <LiquidCrystal.h>
#include "DS1302RTC.h"
#include "tstat.h"

extern LiquidCrystal *lcd;
extern DS1302RTC *rtc;

extern bool     gv_rot_active;
extern bool     gv_long_sw;
extern bool     gv_short_sw;
extern bool     gv_rot_pos;
extern bool     gv_rot_neg;

extern unsigned long lcd_led_strt;
extern unsigned long disptime;

extern int      gv_year_val;
extern char     gv_mon_val[4];
extern int      gv_day_val;
extern int      gv_curr_time;

extern bool     gv_input_active;

extern int      gv_curr_screen;

extern int      gv_temp_set;
extern int      gv_hyst_set;

extern int      gv_mode_set;
extern int      gv_prof_set;

extern int      gv_tint_curr;
extern int      gv_tint_last;

extern int      gv_hint_curr;
extern int      gv_hint_last;

extern int      gv_text_curr;
extern int      gv_text_last;

extern int      gv_hext_curr;
extern int      gv_hext_last;

extern int      gv_curr_scale;

extern prof_params gv_profile[PROF_MAX];

#endif
