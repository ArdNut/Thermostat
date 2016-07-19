//-------------------------------------------------------------------
// tstat_util.cpp
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
#include "tstat_util.h"

// Enable/display the LCD LED
void DispLED(bool state)
{
    if (state) {
        digitalWrite(LCD_LED, HIGH);    // LED ON
    }
    else {
        digitalWrite(LCD_LED, LOW);     // LED OFF
    }
}


// from http://playground.arduino.cc/Code/AvailableMemory
int freeRam()
{
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
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

    if (rtc->set(t) == 0) {
        setTime(t);
        rc = true;
        delay(100);
    }

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


// Need to find the value 200 in addr 0 to verify that EEPROM
// has valid data
bool checkEEPROM()
{
    if (eeprom->read(EE_USED) != 200)
        return false;
    else
        return true;
}


bool enableEEPROM()
{
    eeprom->write(EE_USED, 200);
}


// borrowed from http://forum.arduino.cc/index.php?topic=41497.0
void EEPROM_writeDouble(int addr, double value)
{
   byte* p = (byte*)(void*)&value;
   for (int i = 0; i < sizeof(value); i++)
       eeprom->write(addr++, *p++);
}


double EEPROM_readDouble(int addr)
{
   double value = 0.0;
   byte* p = (byte*)(void*)&value;
   for (int i = 0; i < sizeof(value); i++)
       *p++ = eeprom->read(addr++);
   return value;
}


// NOTE: I've used the original write and read methods instead of the
// newer get and put because one of my development systems is still
// running an older version of the IDE and toolchain.
void saveGVData()
{
    enableEEPROM(); // mark EEPROM as valid
    EEPROM_writeDouble(EE_TSET, gv_temp_set);
    delay(50);
    EEPROM_writeDouble(EE_CSET, gv_hyst_set);
    delay(50);
    eeprom->write(EE_OSET, gv_over_time);
    delay(50);
    eeprom->write(EE_MSET, gv_mode_set);
    delay(50);
    eeprom->write(EE_PSET, gv_prof_set);
    delay(50);
    eeprom->write(EE_TSCL, gv_curr_scale);
    delay(50);
}


void readGVData()
{
    gv_temp_set  = EEPROM_readDouble(EE_TSET);
    if (gv_temp_set == NAN)
        gv_temp_set = 22.0;
    gv_hyst_set  = EEPROM_readDouble(EE_CSET);
    if (gv_hyst_set == NAN)
        gv_hyst_set = 4.0;
    gv_over_time = eeprom->read(EE_OSET);
    gv_mode_set  = eeprom->read(EE_MSET);
    gv_prof_set  = eeprom->read(EE_PSET);
    gv_curr_scale= eeprom->read(EE_TSCL);
}


void writeProfData(int addr, uint8_t prof_id)
{
   byte* p = (byte*)(void*)&gv_profile[prof_id];
   for (int i = 0; i < sizeof(gv_profile[prof_id]); i++)
       eeprom->write(addr++, *p++);
}


void readProfData(int addr, uint8_t prof_id)
{
   byte* p = (byte*)(void*)&gv_profile[prof_id];
   for (int i = 0; i < sizeof(gv_profile[prof_id]); i++)
       *p++ = eeprom->read(addr++);
}


// write profile data to EEPROM
void saveProf(uint8_t prof_id)
{
    switch(prof_id) {
        case PROF_OFF:
            writeProfData(EE_PDATA0, prof_id);
            break;
        case PROF_DAY:
            writeProfData(EE_PDATA1, prof_id);
            break;
        case PROF_NITE:
            writeProfData(EE_PDATA2, prof_id);
            break;
        case PROF_WKND:
            writeProfData(EE_PDATA3, prof_id);
            break;
        default:
            return; // do nothing if invalid ID
            break;
    }
}


// load profile data from EEPROM
void readProf(uint8_t prof_id)
{
    switch(prof_id) {
        case PROF_OFF:
            readProfData(EE_PDATA0, prof_id);
            break;
        case PROF_DAY:
            readProfData(EE_PDATA1, prof_id);
            break;
        case PROF_NITE:
            readProfData(EE_PDATA2, prof_id);
            break;
        case PROF_WKND:
            readProfData(EE_PDATA3, prof_id);
            break;
        default:
            return; // do nothing if invalid ID
            break;
    }
}


// Used to reset EEPROM to default values if nesc.
// Called by pressing the "up" button on the LCD PCB
void resetGVData()
{
    int i;

    lcd->clear();
    lcd->print("SYSTEM RESET");
    delay(1000);

    gv_temp_set   = DEF_TEMP_SET;
    gv_hyst_set   = DEF_HYST_SET;
    gv_over_time  = DEF_OVERTM;
    gv_mode_set   = DEF_MODE;
    gv_prof_set   = DEF_PROF;
    gv_curr_scale = SCALE_F;

    saveGVData();

    for (i = 0; i < PROF_MAX; i++) {
        gv_profile[i].op_mode   = gv_mode_set;
        gv_profile[i].start_hr  = 0;
        gv_profile[i].start_min = 0;
        gv_profile[i].duration  = 0;
        gv_profile[i].tset      = gv_temp_set;
        gv_profile[i].hystrange = gv_hyst_set;

        saveProf(i);
        delay(50);
    }

    lcd->clear();
    lcd->print("RESET COMPLETE");
    delay(1000);
    DispLED(false);
    delay(1000);
    DispLED(true);
    delay(1000);
}
