//-------------------------------------------------------------------
// tstat_gv.h
//
// Created for "Arduino in a Nutshell", 2015, J. M. Hughes
// Chapter 12
//-------------------------------------------------------------------

#ifndef TSTATGV_H
#define TSTATGV_H

#include <LiquidCrystal.h>
#include "DS1302RTC.h"
#include "tstat.h"

extern LiquidCrystal *lcd;
extern DS1302RTC *rtc;

extern bool     rot_active;
extern bool     long_sw;
extern bool     short_sw;
extern bool     rot_pos;
extern bool     rot_neg;

extern unsigned long lcd_led_strt;
extern unsigned long disptime;

extern int      year_val;
extern char     mon_val[4];
extern int      day_val;
extern int      curr_time;

extern bool     input_active;

extern int      curr_screen;

extern int      temp_set;
extern int      hyst_set;

extern int      mode_set;
extern int      prof_set;

extern int      tint_curr;
extern int      tint_last;

extern int      hint_curr;
extern int      hint_last;

extern int      text_curr;
extern int      text_last;

extern int      hext_curr;
extern int      hext_last;

extern int      curr_scale;

extern prof_params profile[PROF_MAX];

#endif
