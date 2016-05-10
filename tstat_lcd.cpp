//-------------------------------------------------------------------
// tstat_lcd.cpp
//
// Created for "Arduino in a Nutshell", 2015, J. M. Hughes
// Chapter 12
//-------------------------------------------------------------------

#include "Arduino.h"
#include <LiquidCrystal.h>
#include "tstat.h"
#include "tstat_gv.h"
#include "tstat_lcd.h"
#include "tstat_util.h"

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


void Screen1()
{
    lcd->clear();

    // inside temperature
    lcd->setCursor(0,0);
    lcd->print("T:");

    lcd->setCursor(2,0);
    lcd->print(tint_curr);

    if (tint_curr < 100)
        lcd->setCursor(4,0);
    else
        lcd->setCursor(5,0);

    if (tint_curr > tint_last) {
        lcd->print("^");
    }
    else if (tint_curr == tint_last) {
        lcd->print("=");
    }
    else {
        lcd->print("v");
    }

    // inside humidity
    lcd->setCursor(6,0);
    lcd->print("H:");

    lcd->setCursor(8,0);
    lcd->print(hint_curr);

    if (hint_curr < 100)
        lcd->setCursor(10,0);
    else
        lcd->setCursor(11,0);

    if (hint_curr > hint_last) {
        lcd->print("^");
    }
    else if (hint_curr == hint_last) {
        lcd->print("=");
    }
    else {
        lcd->print("v");
    }

    // current operation mode
    lcd->setCursor(13,0);
    lcd->print("M:");
    lcd->setCursor(15,0);
    lcd->print(mode_set);

    // outside temperature
    lcd->setCursor(0,1);
    lcd->print("E:");

    lcd->setCursor(2,1);
    lcd->print(text_curr);

    if (text_curr < 100)
        lcd->setCursor(4,1);
    else
        lcd->setCursor(5,1);

    if (text_curr > text_last) {
        lcd->print("^");
    }
    else if (text_curr == text_last) {
        lcd->print("=");
    }
    else {
        lcd->print("v");
    }

    // outside humidity
    lcd->setCursor(6,1);
    lcd->print("H:");

    lcd->setCursor(8,1);
    lcd->print(hext_curr);

    if (hext_curr < 100)
        lcd->setCursor(10,1);
    else
        lcd->setCursor(11,1);

    if (hext_curr > hext_last) {
        lcd->print("^");
    }
    else if (hext_curr == hext_last) {
        lcd->print("=");
    }
    else {
        lcd->print("v");
    }

    // active profile
    lcd->setCursor(13,1);
    lcd->print("P:");
    lcd->setCursor(15,1);
    lcd->print(prof_set);
}


void Screen2()
{
    lcd->clear();

    lcd->setCursor(0,0);
    lcd->print(year_val);

    lcd->setCursor(5,0);
    lcd->print(mon_val);

    lcd->setCursor(9,0);
    lcd->print(day_val);

    // current operation mode
    lcd->setCursor(13,0);
    lcd->print("M:");
    lcd->setCursor(15,0);
    lcd->print(mode_set);

    // print current time
    lcd->setCursor(0,1);
    lcd->print(curr_time);

    if (prof_set) {
        lcd->setCursor(8,1);
        lcd->print("END:");
        lcd->setCursor(12,1);
        lcd->print(GetEndTime());
    }
}


void Screen3()
{
}


void Screen4()
{
}


void Screen5()
{
}


void blinkCursor(int row, int col, bool mode)
{
    if (mode) {
        lcd->setCursor(row, col);
        lcd->cursor();
        lcd->blink();
    }
    else {
        lcd->noBlink();
        lcd->noCursor();
    }
}
