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

// The encoder, RTC and display objects are initialized in tstat_gv.cpp as
// enc, rtc and lcd, respectively.

// Define the service() method in the encoder object as the ISR for timer 1
void timerIsr() {
    enc->service();
}


bool setLocalClock()
{
    bool rc = true;
    bool clksetup = false;
    int  clktry = 10;
    int  clkstate = 0;

    lcd->clear();
    TitleDisp("Initializing", "clock", 1000);

    while (!clksetup) {
        delay(1000);
        switch(clkstate) {
            case 0:     // Start clock osc
                rtc->haltRTC(0);
                delay(50);

                // See if RTC is running
                if (rtc->haltRTC()) {
                    clktry--;
                    if (!clktry) {
                        lcd->print("Clock stopped!");
                        clkstate = 4;
                    }
                    else {
                        clkstate = 0;
                    }
                }
                else {
                    lcd->clear();
                    lcd->print("Clock working.");
                    clkstate = 1;
                    clktry = 10;
                }
                break;
            case 1:     // Enable clock write
                lcd->setCursor(0,1);
                rtc->writeEN(0);
                delay(50);

                // See if RTC write is enabled
                if (!rtc->writeEN()) {
                    clktry--;
                    if (!clktry) {
                        lcd->print("Clock write fail");
                        clkstate = 4;
                    }
                    else {
                        clkstate = 1;
                    }
                }
                else {
                    lcd->clear();
                    lcd->print("Clock write OK");
                    clkstate = 2;
                    clktry = 10;
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
                    }
                    else
                        clkstate = 2;
                }
                else {
                    lcd->clear();
                    lcd->print("Time sync OK");
                    clkstate = 3;
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
                delay(5000);
                break;
        }
    }
}


// Read and decode LCD sheild button inputs
int LCDbutton()
{
    //                        RIGHT, UP, DOWN, LEFT, SELECT
    static int adc_btn_val[5] = {30, 150, 360, 535, 760};
    int input;

    input = analogRead(0);

    if (input < adc_btn_val[0])
        return LCD_BTN_RIGHT;
    else if (input > adc_btn_val[0] && input <= adc_btn_val[1])
        return LCD_BTN_UP;
    else if (input > adc_btn_val[1] && input <= adc_btn_val[2])
        return LCD_BTN_DOWN;
    else if (input > adc_btn_val[2] && input <= adc_btn_val[3])
        return LCD_BTN_LEFT;
    else if (input > adc_btn_val[4] && input <= adc_btn_val[4])
        return LCD_BTN_SELECT;
    else
        return LCD_BTN_NONE; // No valid button pressed
}


void setup()
{


    pinMode(RY1, OUTPUT);
    pinMode(RY2, OUTPUT);
    pinMode(RY3, OUTPUT);
    pinMode(RY4, OUTPUT);

    setRelay(0, false);
    setRelay(1, false);
    setRelay(2, false);
    setRelay(3, false);

    pinMode(LCD_LED, OUTPUT);
    digitalWrite(LCD_LED, HIGH);

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

void displayActive()
{
    DispLED(true);
    lcd->display();
    gv_disp_active = true;
    gv_lcd_led_strt = millis();
}

void loop()
{
    // Check the buttons on the LCD shield
    int lcd_button = -1;

    lcd_button = LCDbutton();

    // Only do one button event no matter how long button is held
    // down. Reset the button flag when button is released.

    if (lcd_button != LCD_BTN_NONE) {
        displayActive();    // enable display if button pressed
        if (!gv_aux_button) {
            gv_aux_button = true;
            gv_show_adj = true;         // enable adj values display
            if (lcd_button == LCD_BTN_SELECT) {
                resetGVData();
                gv_show_adj = false;    // but not for this
            }
            else if (lcd_button == LCD_BTN_UP)
                gv_temp_adj += 0.5;
            else if (lcd_button == LCD_BTN_DOWN)
                gv_temp_adj -= 0.5;
            else if (lcd_button == LCD_BTN_RIGHT)
                gv_hum_adj++;
            else if (lcd_button == LCD_BTN_LEFT)
                gv_hum_adj--;
        }
    }
    else
        // if no button active then reset the button flag
        gv_aux_button = false;

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

        if (lcd_button == LCD_BTN_NONE) {
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

