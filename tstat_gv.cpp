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
#include <ClickEncoder.h>
#include <TimerOne.h>
#include <DHT22.h>
#include <EEPROM.h>

// initialize the library objects
LiquidCrystal   *lcd = new LiquidCrystal(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
DS1302RTC       *rtc = new DS1302RTC(RTC_CE, RTC_IO, RTC_SCLK);
ClickEncoder    *enc = new ClickEncoder(ROT_A, ROT_B, ROT_SW, ROT_STEP, false);
DHT22           *DHT = new DHT22(DHT1);
EEPROMClass     *eeprom = new EEPROMClass;

// Note: acceleration is disabled for the rotary encoder. It's not
// needed for the thermostate.

bool        gv_rot_active = false;      // Encoder turned or pressed
bool        gv_long_sw    = false;      // Long switch action detected
bool        gv_short_sw   = false;      // Short switch action detected
bool        gv_rot_pos    = false;      // CW rotation
bool        gv_rot_neg    = false;      // CCW rotation
bool        gv_dblclick   = false;      // enc button double-click

// The millisecond counter will roll over after about 50 days
// so the code will need to account for that to prevent the LED
// backlight from becoming "hung" in the on state.
unsigned long gv_lcd_led_strt = 0;      // LED backlight start time
unsigned long gv_disptime = 0;          // Screens 1/2 display timer

bool        gv_disp_active = false;     // true if main display active
bool        gv_showscreen  = false;     // Edit display show once gate
bool        gv_saw_button  = false;     // Control input switch gate

bool        gv_show3Aonce  = false;     // display show once gate
bool        gv_show3Bonce  = false;     // display show once gate

uint8_t     gv_scr3npstate = SC3NPNULL; // No profile input screen 3 state
uint8_t     gv_scr3prstate = SC3PRNULL; // Profile active input screen 3 state

uint8_t     gv_scr4state   = SC4NULL;   // Input screen 4 state
uint8_t     gv_sc4mode     = SHOW_PSET;

uint8_t     gv_scr5state   = SC5NULL;   // Input screen 5 state

bool        gv_inputchange  = false;    // param change flag
bool        gv_params_changed = false;  // change indicator flag
int         gv_pchange_time = 0;        // change ind show time

bool        gv_aux_button = false;

uint16_t    gv_year_val   = 2016;       // year
uint8_t     gv_mon_num    = 1;          // month number [1..12]
char        gv_mon_val[4] = {"NUL"};    // month name
uint8_t     gv_day_val    = 1;          // day

tmElements_t gv_rtcvals;                // used to set RTC

uint8_t     gv_currhour   = 0;          // Current time values
uint8_t     gv_currmin    = 0;          //
uint8_t     gv_currsec    = 0;          //

bool        gv_input_active=false;      // true if input entry active

uint8_t     gv_curr_screen = 0;         // current active display screen
uint8_t     gv_dhtchk    = DHT_ERROR_NONE;  // DHT response status

double      gv_temp_set  = DEF_TEMP_SET;    // Temperature set point in C
double      gv_hyst_set  = DEF_HYST_SET;    // Hysteresis range in C
uint8_t     gv_over_time = DEF_OVERTM;      // Override time [0..255]

bool        gv_show_adj  = false;       // control adj values display
double      gv_temp_adj  = 0;           // Temp adjustment in C
int         gv_hum_adj   = 0;           // Humidity adjustment in %RH

uint8_t     gv_mode_set  = DEF_MODE;    // Operation mode
uint8_t     gv_prof_set  = DEF_PROF;    // Profile ID
uint8_t     gv_prof_get  = 0;           // Profile edit ID

double      gv_tint_curr = 0.0;         // Interior temperature in C
double      gv_tint_last = 0.0;         // Interior temperature in C

int         gv_hint_curr = 0;           // Interior humidity
int         gv_hint_last = 0;           // Interior humidity

bool        gv_ext_sense = false;       // true if ext DHT22 detected

double      gv_text_curr = 0.0;         // External temperature in C
double      gv_text_last = 0.0;         // External temperature in C

int         gv_hext_curr = 0;           // External humidity
int         gv_hext_last = 0;           // External humidity

uint8_t     gv_curr_scale = SCALE_F;    // Temp display scale

prof_params gv_profile[PROF_MAX];       // Profiles
prof_params gv_profget;                 // Current profile data

uint8_t     gv_dispstate = DS0;

// temp vars used for EEPROM read and write
uint8_t     gv_tempvar0  = 0;
uint8_t     gv_tempvar1  = 0;

