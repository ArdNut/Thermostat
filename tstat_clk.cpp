//-------------------------------------------------------------------
// tstat_clk.cpp
//
// Created for "Arduino: A Technical Reference", 2015, J. M. Hughes
// Chapter 12
//-------------------------------------------------------------------

#include "Arduino.h"
#include <Time.h>
#include <DS1302RTC.h>
#include <EEPROM.h>

#include "tstat.h"
#include "tstat_gv.h"
#include "tstat_lcd.h"
#include "tstat_clk.h"

bool setLocalClock()
{
    bool rc = true;
    bool clksetup = false;
    bool endclkset = false;
    int  clktry = 10;
    int  clkstate = 0;

    lcd->clear();
    TitleDisp("Initializing", "clock", 1000);

    while (!clksetup && !endclkset) {
        delay(1000);
        switch(clkstate) {
            case 0:
                rtc->haltRTC(0);            // 0 = start clock osc
                delay(200);
                lcd->setCursor(0,1);

                // See if RTC is running
                if (rtc->haltRTC()) {
                    clktry--;
                    if (!clktry) {
                        lcd->print("Clock stopped!");
                        delay(1000);
                        clkstate = 4;
                    }
                    else {
                        clkstate = 0;
                    }
                }
                else {
                    // haltRTC() returned zero, clock is running
                    lcd->clear();
                    lcd->print("Clock working");
                    clkstate = 1;
                    clktry = 10;            // reset attempt counter
                    delay(500);
                }
                break;
            case 1:
                lcd->clear();
                lcd->print("Clock write chk");
                lcd->setCursor(0,1);
                rtc->writeEN(0);            // 0 = enable clock write
                delay(100);

                // See if RTC write is enabled
                if (rtc->writeEN()) {
                    clktry--;               // attempt counter
                    if (!clktry) {
                        lcd->print("Clock write fail");
                        delay(1000);
                        clkstate = 4;
                    }
                    else {
                        clkstate = 1;       // retry
                    }
                }
                else {
                    lcd->clear();
                    lcd->print("Clock write OK");
                    clkstate = 2;
                    clktry = 10;
                    delay(500);
                }
                break;
            case 2:     // set RTC as time lib source
                lcd->clear();
                lcd->print("RTC Sync");
                setSyncProvider(rtc->get);
                delay(50);

                lcd->setCursor(0,1);
                if (timeStatus() != timeSet) {
                    clktry--;
                    if (!clktry) {
                        lcd->print("Time sync FAIL!");
                        clkstate = 4;
                        delay(500);
                    }
                    else
                        clkstate = 2;
                }
                else {
                    lcd->clear();
                    lcd->print("Time sync OK");
                    clkstate = 3;
                    delay(500);
                }

                break;
            case 3:     // Success, exit loop
                clksetup = true;
                break;
            case 4:     // Failure, RTC has issues
                lcd->clear();
                lcd->print("RTC init fail");
                lcd->setCursor(0,1);
                lcd->print("Reset needed");
                delay(500);
                clksetup = true;    // exit anyway
                break;
        }
    }
}


int GetEndTime()
{
    return 0;
}


bool setTstatTime()
{
    bool rc = false;
    time_t t;
    tmElements_t tm;

    if (gv_year_val >= 1000)
        tm.Year = CalendarYrToTm(gv_year_val);
    else    //(year < 100)
        tm.Year = y2kYearToTm(gv_year_val);
    tm.Month  = gv_mon_num;
    tm.Day    = gv_day_val;
    tm.Hour   = gv_currhour;
    tm.Minute = gv_currmin;
    tm.Second = 0;
    t = makeTime(tm);

    setTime(t);
    if (rtc->set(t) == 0) {
        rc = true;
    }
    delay(100);

    return rc;
}


// Get current date and time from the RTC module via
// the Time library (due to the RTC sync in setup())
void RTCUpdate()
{
    gv_currhour = hour();
    gv_currmin  = minute();
    gv_currsec  = second(); // not currently used
    gv_year_val = year();
    gv_mon_num  = month();
    gv_day_val  = day();

    switch (gv_mon_num) {
        case 1:
            gv_mon_val[0] = 'J';
            gv_mon_val[1] = 'A';
            gv_mon_val[2] = 'N';
            break;
        case 2:
            gv_mon_val[0] = 'F';
            gv_mon_val[1] = 'E';
            gv_mon_val[2] = 'B';
            break;
        case 3:
            gv_mon_val[0] = 'M';
            gv_mon_val[1] = 'A';
            gv_mon_val[2] = 'R';
            break;
        case 4:
            gv_mon_val[0] = 'A';
            gv_mon_val[1] = 'P';
            gv_mon_val[2] = 'R';
            break;
        case 5:
            gv_mon_val[0] = 'M';
            gv_mon_val[1] = 'A';
            gv_mon_val[2] = 'Y';
            break;
        case 6:
            gv_mon_val[0] = 'J';
            gv_mon_val[1] = 'U';
            gv_mon_val[2] = 'N';
            break;
        case 7:
            gv_mon_val[0] = 'J';
            gv_mon_val[1] = 'U';
            gv_mon_val[2] = 'L';
            break;
        case 8:
            gv_mon_val[0] = 'A';
            gv_mon_val[1] = 'U';
            gv_mon_val[2] = 'G';
            break;
        case 9:
            gv_mon_val[0] = 'S';
            gv_mon_val[1] = 'E';
            gv_mon_val[2] = 'P';
            break;
        case 10:
            gv_mon_val[0] = 'O';
            gv_mon_val[1] = 'C';
            gv_mon_val[2] = 'T';
            break;
        case 11:
            gv_mon_val[0] = 'N';
            gv_mon_val[1] = 'O';
            gv_mon_val[2] = 'V';
            break;
        case 12:
            gv_mon_val[0] = 'D';
            gv_mon_val[1] = 'E';
            gv_mon_val[2] = 'C';
            break;
        default:
            gv_mon_val[0] = 'E';
            gv_mon_val[1] = 'R';
            gv_mon_val[2] = 'R';
            break;
    }
}
