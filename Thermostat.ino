//-------------------------------------------------------------------
// Thermostat.ino
//
// Created for "Arduino: A Technical Reference", 2015, J. M. Hughes
// Chapter 12
//-------------------------------------------------------------------

// Include all modules

#include <stdint.h>
#include <LiquidCrystal.h>
#include <Time.h>
#include <DS1302RTC.h>
#include <ClickEncoder.h>
#include <TimerOne.h>
#include <DHT22.h>
#include <EEPROM.h>

#include "tstat.h"
#include "tstat_util.h"
#include "tstat_ctrl.h"
#include "tstat_gv.h"
#include "tstat_iface.h"
#include "tstat_lcd.h"
#include "tstat_clk.h"

// The encoder, RTC and display objects are initialized in tstat_gv.cpp as
// enc, rtc and lcd, respectively.

// Define the service() method in the encoder object as the ISR for timer 1
void timerIsr() {
    enc->service();
}


void setup()
{
    pinMode(RY1, OUTPUT);   // Relay 1 output
    pinMode(RY2, OUTPUT);   // Relay 2 output
    pinMode(RY3, OUTPUT);   // Relay 3 output
    pinMode(RY4, OUTPUT);   // Relay 4 output

    setRelay(0, false);     // Set all relays OFF
    setRelay(1, false);
    setRelay(2, false);
    setRelay(3, false);

    pinMode(LCD_LED, OUTPUT);       // LCD backlight LED
    digitalWrite(LCD_LED, HIGH);    // Enable the backlight

    // Initialize timer 1 and the ISR
    Timer1.initialize(1000);
    Timer1.attachInterrupt(timerIsr);

    // First-time fetch of temp and humidity
    GetTemps();

    lcd->begin(16, 2);  // Set the dimensions of the LCD

    lcd->display();

    TitleDisp("AATR Thermostat","V1.0",1000);

    setLocalClock();
    delay (1000);

    // Setup time library with RTC
    lcd->setCursor(0,1);
    if (timeStatus() != timeSet)
        lcd->print("Time init error");
    delay (1000);

    // Get saved parameters from EEPROM if set
    if (checkEEPROM())
        readGVData();

    TitleDisp("Initialization", "complete", 1000);

    gv_curr_screen = 0;
    Screen1(false);      // Show screen 1
    gv_disptime = millis();
    gv_lcd_led_strt = millis();
    gv_disp_active = true;
}


void loop()
{
    // Check LCD buttons
    CheckLCDButtons();

    // Update temp and humidity
    GetTemps();

    // Check for active input
    gv_input_active = HandleInput();

    if (!gv_input_active) {
        // Get current date and time from RTC
        RTCUpdate();

        // Reset the input screen states
        gv_dispstate   = DS0;
        gv_scr3npstate = SC3NPNULL;
        gv_scr3prstate = SC3PRNULL;
        gv_scr4state   = SC4NULL;
        gv_scr5state   = SC5NULL;

        // Check for LCD button active, only proceed if not
        if (gv_lcd_button == LCD_BTN_NONE) {
            if (gv_disp_active) {
                DispLED(true);
                // If input not active, show status displays
                // Toggle between screen 1 and screen 2
                if ((millis() - gv_disptime) > MAX_DISP_TIME) {
                    if (gv_curr_screen) {
                        Screen1(gv_show_adj);
                        gv_curr_screen = 0;
                        gv_disptime = millis();
                    }
                    else {
                        Screen2();
// Uncomment the next two lines to see approximately how much RAM is
// available.
//                        lcd->setCursor(6,1);    // TESTING
//                        lcd->print(freeRam());  // TESTING
                        gv_curr_screen = 1;
                        gv_disptime = millis();
                    }
                }
            }
        }
    }
    else {
        // Enable display if control input
        displayActive();
    }

    // Disable display after idle for MAX_DISP_TIME
    if (!gv_input_active && gv_disp_active) {
        if ((millis() - gv_lcd_led_strt) >= MAX_ILUM_TIME) {
            gv_disp_active = false;
            lcd->noDisplay();
            DispLED(false);
            gv_show_adj = false; // disable adj display if on
        }
    }

    // Get current settings from EEPROM
    if (!gv_input_active) {
        // anything changed?
        if (gv_inputchange) {
            readGVData();
            gv_inputchange = false;     // reset gate flag
            gv_params_changed = true;   // enable change indicator
            gv_pchange_time = millis();
        }
    }

    // disable the change indicator on LCD
    if (gv_params_changed) {
        if ((millis() - gv_pchange_time) > 10000) {
            gv_params_changed = false;
        }
    }

    // Profile update
    CheckProfile();

    // Relay control
    // Only do system control if control input is not active
    SystemControl();

    delay(250);
}

