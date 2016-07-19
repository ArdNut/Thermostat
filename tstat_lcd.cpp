//-------------------------------------------------------------------
// tstat_lcd.cpp
//
// Created for "Arduino: A Technical Reference", 2015, J. M. Hughes
// Chapter 12
//-------------------------------------------------------------------

#include "Arduino.h"
#include <LiquidCrystal.h>
#include "tstat.h"
#include "tstat_gv.h"
#include "tstat_lcd.h"
#include "tstat_util.h"
#include "tstat_ctrl.h"

void showMode(int col, int row)
{
    lcd->setCursor(col, row);
    switch (gv_mode_set) {
        case MODE_OFF:
            lcd->print("OFF ");
            break;
        case MODE_COOL:
            lcd->print("COOL");
            break;
        case MODE_HEAT:
            lcd->print("HEAT");
            break;
        case MODE_FAN:
            lcd->print("FAN ");
            break;
        case MODE_AUTO:
            lcd->print("AUTO");
            break;
    }
}


void showProf(int col, int row)
{
    int prof_id;
    if (gv_sc4mode == SHOW_PGET)
        prof_id = gv_prof_get;
    else
        prof_id = gv_prof_set;

    lcd->setCursor(col, row);
    switch (prof_id) {
        case PROF_OFF:
            lcd->print("X");
            break;
        case PROF_DAY:
            lcd->print("D");
            break;
        case PROF_NITE:
            lcd->print("N");
            break;
        case PROF_WKND:
            lcd->print("W");
            break;
    }
}


// Display two rows of text, dwell for dtime milliseconds
void TitleDisp(char *msg1, char *msg2, int dtime)
{
    lcd->clear();

    lcd->setCursor(0,0);
    lcd->print(msg1);
    delay(dtime);

    lcd->setCursor(0,1);
    lcd->print(msg2);
    delay(dtime);
}


// Main status screen
// The position of the '=', 'V', and '^' chars will automatically shift
// over if the temp or hum values are >= 100
void Screen1()
{
    lcd->clear();

    // inside temperature
    lcd->setCursor(0,0);
    lcd->print("T:");

    lcd->setCursor(2,0);
    lcd->print(convtemp((double) gv_tint_curr));

    if (gv_tint_curr < 100)
        lcd->setCursor(4,0);
    else
        lcd->setCursor(5,0);

    if (gv_tint_curr > gv_tint_last) {
        lcd->print("^");
    }
    else if (gv_tint_curr == gv_tint_last) {
        lcd->print("=");
    }
    else {
        lcd->print("v");
    }

    // inside humidity
    lcd->setCursor(6,0);
    lcd->print("H:");

    lcd->setCursor(8,0);
    lcd->print(gv_hint_curr);

    if (gv_hint_curr < 100)
        lcd->setCursor(10,0);
    else
        lcd->setCursor(11,0);

    if (gv_hint_curr > gv_hint_last) {
        lcd->print("^");
    }
    else if (gv_hint_curr == gv_hint_last) {
        lcd->print("=");
    }
    else {
        lcd->print("v");
    }

    // current operation mode
    showMode(12, 0);

    if (gv_ext_sense) {
        // outside temperature
        lcd->setCursor(0,1);
        lcd->print("E:");

        lcd->setCursor(2,1);
        lcd->print(convtemp((double) gv_text_curr));

        if (gv_text_curr < 100)
            lcd->setCursor(4,1);
        else
            lcd->setCursor(5,1);

        if (gv_text_curr > gv_text_last) {
            lcd->print("^");
        }
        else if (gv_text_curr == gv_text_last) {
            lcd->print("=");
        }
        else {
            lcd->print("v");
        }

        // outside humidity
        lcd->setCursor(6,1);
        lcd->print("H:");

        lcd->setCursor(8,1);
        lcd->print(gv_hext_curr);

        if (gv_hext_curr < 100)
            lcd->setCursor(10,1);
        else
            lcd->setCursor(11,1);

        if (gv_hext_curr > gv_hext_last) {
            lcd->print("^");
        }
        else if (gv_hext_curr == gv_hext_last) {
            lcd->print("=");
        }
        else {
            lcd->print("v");
        }
    }

    if (gv_params_changed) {
        lcd->setCursor(12,1);
        lcd->print("*");
    }
    else {
        lcd->setCursor(12,1);
        lcd->print(" ");
    }

    // active profile
    lcd->setCursor(13,1);
    lcd->print("P:");
    showProf(15, 1);
}

// Alternate main screen
void Screen2()
{
    lcd->clear();

    lcd->setCursor(0,0);
    lcd->print(gv_year_val);

    lcd->setCursor(5,0);
    lcd->print(gv_mon_val);

    lcd->setCursor(9,0);
    lcd->print(gv_day_val);

    // current operation mode
    lcd->setCursor(13,0);
    lcd->print("M:");
    lcd->setCursor(15,0);
    lcd->print(gv_mode_set);

    // print current time
    lcd->setCursor(0,1);
    lcd->print("0000");

    if (gv_currhour < 10)
        lcd->setCursor(1,1);
    else
        lcd->setCursor(0,1);
    lcd->print(gv_currhour);

    if (gv_currmin < 10)
        lcd->setCursor(3,1);
    else
        lcd->setCursor(2,1);
    lcd->print(gv_currmin);

    if (gv_prof_set) {
        lcd->setCursor(8,1);
        lcd->print("END:");
        lcd->setCursor(12,1);
        lcd->print(GetEndTime());
    }
}

// Primary settings
void Screen3(int mode)
{
    lcd->clear();

    if (mode == SHOW_ALL) {
        lcd->setCursor(0,0);
        lcd->print("S:");
        lcd->print(convtemp(gv_temp_set));

        // current operation mode
        showMode(8, 0);
        lcd->setCursor(13,0);
        lcd->print("M:");
        lcd->setCursor(15,0);
        lcd->print(gv_mode_set);

        lcd->setCursor(0,1);
        lcd->print("C:");
        lcd->print(convrange(gv_hyst_set));

        lcd->setCursor(6,1);
        lcd->print("T:");
        lcd->print(gv_over_time);
    }

    lcd->setCursor(5,0);
    if (gv_curr_scale == SCALE_F)
        lcd->print("F");
    else
        lcd->print("C");

    lcd->setCursor(13,1);
    lcd->print("P:");
    showProf(15, 1);
}


// Profile editing
void Screen4(int mode)
{
    int prof_id;

    if (mode == SHOW_PSET)
        prof_id = gv_prof_set;
    else
        prof_id = gv_prof_get;

    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print("P:");
    showProf(2, 0);

    if (prof_id > PROF_OFF) {
        lcd->setCursor(4, 0);
        lcd->print("B:");
        lcd->print(gv_profile[prof_id].start_hr);

        lcd->setCursor(11, 0);
        lcd->print("S:");
        lcd->print(convtemp(gv_profile[prof_id].tset));

        lcd->setCursor(0, 1);
        lcd->print("M:");
        lcd->print(gv_profile[prof_id].start_min);

        lcd->setCursor(4, 1);
        lcd->print("E:");
        lcd->print(gv_profile[prof_id].duration);

        lcd->setCursor(11, 1);
        lcd->print("C:");
        lcd->print(convtemp(gv_profile[prof_id].hystrange));
    }
    else {
        lcd->setCursor(0,1);
        lcd->print("No profile set");
    }
    lcd->setCursor(0,0);
}

// RTC setup
void Screen5()
{
    lcd->clear();
    lcd->setCursor(0,0);

    lcd->print("Y:");
    lcd->print(gv_year_val);

    lcd->setCursor(7,0);
    lcd->print("M:");
    lcd->print(gv_mon_num);

    lcd->setCursor(12,0);
    lcd->print("D:");
    lcd->print(gv_day_val);

    lcd->setCursor(0,1);
    lcd->print("H:00");
    if (gv_currhour < 10)
        lcd->setCursor(3,1);
    else
        lcd->setCursor(2,1);
    lcd->print(gv_currhour);

    lcd->setCursor(5,1);
    lcd->print("M:00");
    if (gv_currmin < 10)
        lcd->setCursor(8,1);
    else
        lcd->setCursor(7,1);
    lcd->print(gv_currmin);
}


void blinkCursor(int col, int row, bool mode)
{
    if (mode) {
        lcd->setCursor(col, row);
//        lcd->cursor();
        lcd->blink();
    }
    else {
        lcd->noBlink();
//        lcd->noCursor();
    }
}
