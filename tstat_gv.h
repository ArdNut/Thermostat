//-------------------------------------------------------------------
// tstat_gv.h
//
// Created for "Arduino: A Technical Reference", 2015, J. M. Hughes
// Chapter 12
//-------------------------------------------------------------------

#ifndef TSTATGV_H
#define TSTATGV_H

#include <LiquidCrystal.h>
#include <ClickEncoder.h>
#include <DHT22.h>
#include <DS1302RTC.h>
#include <EEPROM.h>

#include "tstat.h"

extern LiquidCrystal *lcd;
extern DS1302RTC     *rtc;
extern ClickEncoder  *enc;
extern DHT22         *DHT;
extern EEPROMClass   *eeprom;

extern bool          gv_rot_active;
extern bool          gv_long_sw;
extern bool          gv_short_sw;
extern bool          gv_rot_pos;
extern bool          gv_rot_neg;
extern bool          gv_dblclick;

extern unsigned long gv_lcd_led_strt;
extern unsigned long gv_disptime;

extern bool          gv_disp_active;
extern bool          gv_showscreen;
extern bool          gv_saw_button;

extern bool          gv_show3Aonce;
extern bool          gv_show3Bonce;

extern uint8_t       gv_scr3npstate;
extern uint8_t       gv_scr3prstate;

extern uint8_t       gv_scr4state;
extern uint8_t       gv_sc4mode;

extern uint8_t       gv_scr5state;

extern bool          gv_inputchange;
extern bool          gv_params_changed;
extern int           gv_pchange_time;

extern uint16_t      gv_year_val;
extern uint8_t       gv_mon_num;
extern char          gv_mon_val[4];
extern uint8_t       gv_day_val;

extern tmElements_t  gv_rtcvals;

extern uint8_t       gv_currhour;
extern uint8_t       gv_currmin;
extern uint8_t       gv_currsec;

extern bool          gv_input_active;

extern uint8_t       gv_curr_screen;
extern uint8_t       gv_dhtchk;

extern double        gv_temp_set;
extern double        gv_hyst_set;
extern uint8_t       gv_over_time;

extern uint8_t       gv_mode_set;
extern uint8_t       gv_prof_set;
extern uint8_t       gv_prof_get;

extern double        gv_tint_curr;
extern double        gv_tint_last;

extern uint8_t       gv_hint_curr;
extern uint8_t       gv_hint_last;

extern bool          gv_ext_sense;

extern double        gv_text_curr;
extern double        gv_text_last;

extern uint8_t       gv_hext_curr;
extern uint8_t       gv_hext_last;

extern uint8_t       gv_curr_scale;

extern prof_params   gv_profile[PROF_MAX];
extern prof_params   gv_profget;

extern uint8_t       gv_dispstate;

extern uint8_t       gv_tempvar0;
extern uint8_t       gv_tempvar1;

#endif
