//-------------------------------------------------------------------
// Thermostat.ino
//
// Created for "Arduino in a Nutshell", 2015, J. M. Hughes
// Chapter 12
//-------------------------------------------------------------------


// Include all modules

#include <stdint.h>
#include <LiquidCrystal.h>
#include <Time.h>
#include <DS1302RTC.h>

#include "tstat.h"
#include "tstat_util.h"
#include "tstat_ctrl.h"
#include "tstat_gv.h"
#include "tstat_iface.h"
#include "tstat_lcd.h"

void setup()
{
    lcd->begin(16, 2);  // Set the dimensions of the LCD

    TitleDisp("Initializing...", "", 1000);

    lcd->clear();

    if (rtc->haltRTC())
        lcd->print("Clock stopped!");
    else
        lcd->print("Clock working.");

    lcd->setCursor(0,1);
    if (rtc->writeEN())
        lcd->print("Write allowed.");
    else
        lcd->print("Write protected.");

    delay (2000);

    // Setup time library
    lcd->clear();
    lcd->print("RTC Sync");
    setSyncProvider(rtc->get);          // the function to get the time from the RTC
    lcd->setCursor(0,1);
    if (timeStatus() == timeSet)
        lcd->print(" Ok!");
    else
        lcd->print(" FAIL!");

    delay (1000);

    TitleDisp("Initialization", "complete", 1000);

    curr_screen = 0;
    Screen1();
    disptime = millis();
}


void loop()
{
    // Get current date and time from RTC
    RTCUpdate();

    if (input_active) {
        HandleInput();
    }
    else {
        // Toggle between screen 1 and screen 2
        if ((millis() - disptime) > MAX_DISP_TIME) {
            if (curr_screen) {
                Screen1();
                curr_screen = 0;
                disptime = millis();
            }
            else {
                Screen2();
                curr_screen = 1;
                disptime = millis();
            }
        }
    }

    GetTemps();
    SystemControl();
}

