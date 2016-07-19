//-------------------------------------------------------------------
// tstat_iface.cpp
//
// Created for "Arduino: A Technical Reference", 2015, J. M. Hughes
// Chapter 12
//-------------------------------------------------------------------

#include "Arduino.h"
#include "tstat.h"
#include "tstat_gv.h"
#include "tstat_util.h"
#include "tstat_iface.h"
#include "tstat_lcd.h"
#include "tstat_ctrl.h"
#include <ClickEncoder.h>

bool HandleInput()
{
    bool rc = false;
    int16_t encval;

    encval = enc->getValue();   // get encoder value

    ClickEncoder::Button btn = enc->getButton();
    if (btn != ClickEncoder::Open) {
        // if display is off, any input turns it on, but we don't do
        // anything with the input until the display is enabled
        if (!gv_disp_active) {
            gv_saw_button = false;
            return true;
        }

        // Get button input from encoder module
        // button presses take precedence over rotary encoder
        switch (btn) {
            case ClickEncoder::Pressed:
                // just indicates button pressed
                // not the type of action
                break;
            case ClickEncoder::Held:
                if (!gv_saw_button) {
                    gv_saw_button = true;
                    gv_long_sw    = true;
                    gv_short_sw   = false;
                    gv_dblclick   = false;
                }
                break;
            case ClickEncoder::Released:
                // Not used
                break;
            case ClickEncoder::Clicked:
                if (!gv_saw_button) {
                    gv_saw_button = true;
                    gv_long_sw    = false;
                    gv_short_sw   = true;
                    gv_dblclick   = false;
                }
                break;
            case ClickEncoder::DoubleClicked:
                if (!gv_saw_button) {
                    gv_saw_button = true;
                    gv_long_sw    = false;
                    gv_short_sw   = false;
                    gv_dblclick   = true;
                }
                break;
        }
    }
    else {
        gv_long_sw    = false;
        gv_short_sw   = false;
        gv_saw_button = false;
        gv_dblclick   = false;

        // check for encoder input
        if (encval != 0) {
            gv_rot_active = true;
            if (encval > 0) {
                gv_rot_pos = true;
                gv_rot_neg = false;
            }
            else if (encval < 0) {
                gv_rot_pos = false;
                gv_rot_neg = true;
            }
        }
        else {
            gv_rot_active = false;
            gv_rot_pos    = false;
            gv_rot_neg    = false;
        }
    }

    // got input (if any), now call the state machine
    rc = processInput();

    return rc;
}


void nextLCDdisp(int nextstate)
{
    gv_long_sw = false;
    blinkCursor(0, 0, false);
    DispLED(false);
    gv_dispstate = nextstate;
    gv_showscreen = false;
    delay(100);
}


// Display control state machine
// Uses decoded control input to manipulate the display screens and settings.
// gv_input_active will remain true until screen 5 exit.
bool processInput()
{
    bool rc = false;

    switch(gv_dispstate) {
        case DS0:
            // The primary alternating status screens
            if (gv_long_sw && gv_saw_button) {
                nextLCDdisp(DS1);
                rc = true;
            }

            break;
        case DS1:       // Screen 3
            // Settings screen
            if (!gv_showscreen) {
                DispLED(true);
                Screen3(SHOW_ALL);
                gv_showscreen = true;
                gv_saw_button = false;
            }

            if (gv_dblclick && gv_saw_button) {
                gv_dblclick = false;
                nextLCDdisp(DS0);
            }
            else if (gv_long_sw && gv_saw_button) {
                nextLCDdisp(DS2);
            }
            else
                getScreen3();

            rc = true;

            break;
        case DS2:       // Screen 4
            // Profile edit screen
            if (!gv_showscreen) {
                DispLED(true);
                Screen4(gv_sc4mode);
                gv_showscreen = true;
                gv_saw_button = false;
            }

            if (gv_dblclick && gv_saw_button) {
                gv_dblclick = false;
                nextLCDdisp(DS0);
            }
            else if (gv_long_sw && gv_saw_button) {
                nextLCDdisp(DS3);
            }
            else
                getScreen4();

            rc = true;

            break;
        case DS3:       // Screen 5
            // Time/date setup screen
            if (!gv_showscreen) {
                DispLED(true);
                Screen5();
                gv_showscreen = true;
                gv_saw_button = false;
            }

            if (gv_dblclick && gv_saw_button) {
                gv_dblclick = false;
                nextLCDdisp(DS0);
            }
            else if (gv_long_sw && gv_saw_button) {
                nextLCDdisp(DS0);
                rc = false;
            }
            else {
                rc = true;
                getScreen5();
            }

            break;
    }

    return rc;
}


// state change helpers (to reduce duplicate code)
void nextSC3NP(int nextState)
{
    blinkCursor(0, 0, false);
    gv_short_sw = false;
    gv_scr3npstate = nextState;
}


void nextSC3PR(int nextState)
{
    blinkCursor(0, 0, false);
    gv_short_sw = false;
    gv_scr3prstate = nextState;
}


void nextSC4(int nextState)
{
    blinkCursor(0, 0, false);
    gv_short_sw = false;
    gv_scr4state = nextState;
}


void nextSC5(int nextState)
{
    blinkCursor(0, 0, false);
    gv_short_sw = false;
    gv_scr5state = nextState;
}

// Current settings
// Saved as profile entry 0 (profile OFF)
void getScreen3()
{
    gv_sc4mode = SHOW_PSET;

    if (!gv_prof_set) {
        switch(gv_scr3npstate) {
            case SC3NPNULL:
                if (!gv_show3Aonce) {
                    Screen3(SHOW_ALL);
                    gv_show3Aonce = true;
                }

                if (gv_short_sw) {
                    nextSC3NP(SC3NPTEMP);
                }
                break;
            case SC3NPTEMP:         // Set target temp
                blinkCursor(0, 0, true);

                if (gv_rot_active) {
                    gv_rot_active = false;
                    if (gv_rot_pos) {
                        gv_rot_pos = false;
                        if (gv_curr_scale == SCALE_F)
                            gv_temp_set += 0.59;
                        else
                            gv_temp_set += 1;
                    }
                    if (gv_rot_neg) {
                        gv_rot_neg = false;
                        if (gv_curr_scale == SCALE_F)
                            gv_temp_set -= 0.59;
                        else
                            gv_temp_set -= 1;
                    }

                    lcd->setCursor(2, 0);
                    lcd->print(convtemp(gv_temp_set));
                }

                if (gv_short_sw) {
                    nextSC3NP(SC3NPCYCL);
                }
                break;
            case SC3NPCYCL:         // Set hyst range
                lcd->setCursor(0, 1);
                blinkCursor(0, 1, true);

                if (gv_rot_active) {
                    gv_rot_active = false;
                    if (gv_rot_pos) {
                        gv_rot_pos = false;
                        if (gv_curr_scale == SCALE_F)
                            gv_hyst_set += 0.59;
                        else
                            gv_hyst_set += 1.0;
                    }
                    if (gv_rot_neg) {
                        gv_rot_neg = false;
                        if (gv_curr_scale == SCALE_F)
                            gv_hyst_set -= 0.59;
                        else
                            gv_hyst_set -= 1.0;
                    }

                    if (gv_hyst_set == NULL)
                        gv_hyst_set = 0.0;
                    if (gv_hyst_set > 9.0)
                        gv_hyst_set = 0.0;
                    if (gv_hyst_set < 0.0)
                        gv_hyst_set = 9.0;

//        gv_hyst_set = 4.0;
                    lcd->setCursor(2, 1);
                    lcd->print(convrange(gv_hyst_set));
                }
//                    lcd->setCursor(4, 1);
//                    lcd->print(gv_hyst_set);

                if (gv_short_sw) {
                    nextSC3NP(SC3NPTSCL);
                }
                break;
            case SC3NPTSCL:         // Set display scale (C or F)
                blinkCursor(5, 0, true);

                if (gv_rot_active) {
                    gv_rot_active = false;
                    if (gv_rot_pos || gv_rot_neg) {
                        gv_rot_pos = false;
                        gv_rot_neg = false;
                        if (gv_curr_scale == SCALE_F)
                            gv_curr_scale = SCALE_C;
                        else
                            gv_curr_scale = SCALE_F;
                    }
                    lcd->setCursor(5,0);
                    if (gv_curr_scale == SCALE_F)
                        lcd->print("F");
                    else
                        lcd->print("C");
                }

                if (gv_short_sw) {
                    nextSC3NP(SC3NPOVRT);
                }
                break;
            case SC3NPOVRT:
                blinkCursor(6, 1, true);

                if (gv_rot_active) {
                    gv_rot_active = false;
                    if (gv_rot_pos) {
                        gv_rot_pos = false;
                        gv_over_time++;
                    }
                    if (gv_rot_neg) {
                        gv_rot_neg = false;
                        gv_over_time--;
                    }

                    lcd->setCursor(8, 1);
                    lcd->print(gv_over_time);
                }

                if (gv_short_sw) {
                    nextSC3NP(SC3NPMODE);
                }
                break;
            case SC3NPMODE:         // Set operation mode
                blinkCursor(13, 0, true);

                if (gv_rot_active) {
                    gv_rot_active = false;
                    if (gv_rot_pos) {
                        gv_rot_pos = false;
                        gv_mode_set++;
                    }
                    if (gv_rot_neg) {
                        gv_rot_neg = false;
                        gv_mode_set--;
                    }

                    if (gv_mode_set > MODE_AUTO)
                        gv_mode_set = MODE_OFF;
                    if (gv_mode_set < MODE_OFF)
                        gv_mode_set = MODE_AUTO;

                    showMode(8, 0);

                    lcd->setCursor(15,0);
                    lcd->print(gv_mode_set);
                }

                lcd->setCursor(15,0);
                lcd->print(gv_mode_set);
                lcd->setCursor(13,0);

                if (gv_short_sw) {
                    nextSC3NP(SC3NPPROF);
                }
                break;
            case SC3NPPROF:         // Set profile number
                blinkCursor(13, 1, true);

                if (gv_rot_active) {
                    gv_rot_active = false;
                    if (gv_rot_pos) {
                        gv_rot_pos = false;
                        gv_prof_set++;
                    }
                    if (gv_rot_neg) {
                        gv_rot_neg = false;
                        gv_prof_set--;
                    }

                    if (gv_prof_set > PROF_WKND)
                        gv_prof_set = PROF_OFF;
                    if (gv_prof_set < PROF_OFF)
                        gv_prof_set = PROF_WKND;

                    showProf(15, 1);
                    if (gv_prof_set != PROF_OFF) {
                        gv_show3Bonce = false;
                    }
                }

                if (gv_short_sw) {
                    nextSC3NP(SC3NPSET);
                }
                break;

            case SC3NPSET:
                if (gv_prof_set == PROF_OFF) {
                    gv_profile[gv_prof_set].op_mode   = gv_mode_set;
                    gv_profile[gv_prof_set].start_hr  = 0;
                    gv_profile[gv_prof_set].start_min = 0;
                    gv_profile[gv_prof_set].duration  = gv_over_time;
                    gv_profile[gv_prof_set].tset      = gv_temp_set;
                    gv_profile[gv_prof_set].hystrange = gv_hyst_set;

                    lcd->clear();
                    lcd->setCursor(0,0);
                    lcd->print("Settings saved");
                    saveProf(gv_prof_set);
                    saveGVData();
                    gv_inputchange = true;
                    delay(2000);
                }

                Screen3(SHOW_ALL);
                nextSC3NP(SC3NPNULL);
                break;
        }
    }
    else {
        // If any profile is active then only allow edit of the
        // temp units and the profile number
        if (!gv_show3Bonce) {
            Screen3(SHOW_SUB);
            gv_show3Bonce = true;
        }
        switch(gv_scr3prstate) {
            case SC3PRNULL:
                if (gv_short_sw) {
                    nextSC3PR(SC3PRTSCL);
                }
                break;
            case SC3PRTSCL:         // Set display scale (C or F)
                blinkCursor(5, 0, true);

                if (gv_rot_active) {
                    gv_rot_active = false;
                    if (gv_rot_pos || gv_rot_neg) {
                        gv_rot_pos = false;
                        gv_rot_neg = false;
                        if (gv_curr_scale == SCALE_F)
                            gv_curr_scale = SCALE_C;
                        else
                            gv_curr_scale = SCALE_F;
                    }
                    lcd->setCursor(5,0);
                    if (gv_curr_scale == SCALE_F)
                        lcd->print("F");
                    else
                        lcd->print("C");
                }

                if (gv_short_sw) {
                    nextSC3PR(SC3PRPROF);
                }
                break;
            case SC3PRPROF:         // Set profile number
                blinkCursor(13, 1, true);

                if (gv_rot_active) {
                    gv_rot_active = false;
                    if (gv_rot_pos) {
                        gv_rot_pos = false;
                        gv_prof_set++;
                    }
                    if (gv_rot_neg) {
                        gv_rot_neg = false;
                        gv_prof_set--;
                    }

                    if (gv_prof_set > PROF_WKND)
                        gv_prof_set = PROF_OFF;
                    if (gv_prof_set < PROF_OFF)
                        gv_prof_set = PROF_WKND;

                    showProf(15, 1);

                    if (gv_prof_set == PROF_OFF) {
                        gv_show3Aonce = false;
                    }
                }

                if (gv_short_sw) {
                    gv_inputchange = true;
                    nextSC3PR(SC3PRNULL);
                }
                break;
        }
    }
}


void getScreen4()
{
    switch(gv_scr4state) {
        case SC4NULL:
            if (gv_short_sw) {
                gv_prof_get = gv_prof_set;
                nextSC4(SC4PROF);
            }
            break;
        case SC4PROF:
            lcd->setCursor(0, 0);
            blinkCursor(0, 0, true);

            if (gv_rot_active) {
                gv_rot_active = false;
                if (gv_rot_pos) {
                    gv_rot_pos = false;
                    gv_prof_get++;
                }
                if (gv_rot_neg) {
                    gv_rot_neg = false;
                    gv_prof_get--;
                }

                if (gv_prof_get > PROF_WKND)
                    gv_prof_get = PROF_OFF;
                if (gv_prof_get < PROF_OFF)
                    gv_prof_get = PROF_WKND;

                if (gv_prof_get > PROF_OFF)
                    gv_sc4mode = SHOW_PGET;
                else
                    gv_sc4mode = SHOW_PSET;

                showProf(2, 0);
            }

            if (gv_short_sw) {
                if (gv_prof_get > PROF_OFF) {
                    gv_profget.op_mode   = gv_profile[gv_prof_get].op_mode;
                    gv_profget.start_hr  = gv_profile[gv_prof_get].start_hr;
                    gv_profget.start_min = gv_profile[gv_prof_get].start_min;
                    gv_profget.duration  = gv_profile[gv_prof_get].duration;
                    gv_profget.tset      = gv_profile[gv_prof_get].tset;
                    gv_profget.hystrange = gv_profile[gv_prof_get].hystrange;
                    nextSC4(SC4MODE);   // go to next edit state
                }
                else {
                    nextSC4(SC4NULL);   // return to start state
                }
                Screen4(gv_sc4mode);
            }
            break;
        case SC4MODE:
            lcd->setCursor(0, 1);
            blinkCursor(0, 1, true);

            if (gv_rot_active) {
                gv_rot_active = false;
                if (gv_rot_pos) {
                    gv_rot_pos = false;
                    gv_profget.op_mode++;
                }
                if (gv_rot_neg) {
                    gv_rot_neg = false;
                    gv_profget.op_mode--;
                }

                if (gv_profget.op_mode > MODE_AUTO)
                    gv_profget.op_mode = MODE_OFF;
                if (gv_profget.op_mode < MODE_OFF)
                    gv_profget.op_mode = MODE_AUTO;

                lcd->setCursor(2,1);
                lcd->print(gv_profget.op_mode);
            }

            if (gv_short_sw) {
                nextSC4(SC4BTIME);
            }
            break;
        case SC4BTIME:
            blinkCursor(4, 0, true);

            if (gv_rot_active) {
                gv_rot_active = false;
                if (gv_rot_pos) {
                    gv_rot_pos = false;
                    gv_profget.start_min++;
                }
                if (gv_rot_neg) {
                    gv_rot_neg = false;
                    gv_profget.start_min--;
                }

                if (gv_profget.start_min > 59) {
                    gv_profget.start_min = 0;
                    gv_profget.start_hr++;
                    if (gv_profget.start_hr > 23)
                        gv_profget.start_hr = 0;
                }

                if (gv_profget.start_min < 0) {
                    gv_profget.start_min = 59;
                    gv_profget.start_hr--;
                    if (gv_profget.start_hr < 0)
                        gv_profget.start_hr = 23;
                }

                // Show start time in 24 hour format
                lcd->setCursor(6, 0);
                lcd->print("0000");
                if (gv_profget.start_hr) {
                    if (gv_profget.start_hr < 10)
                        lcd->setCursor(7, 0);
                    else
                        lcd->setCursor(6, 0);
                    lcd->print(gv_profget.start_hr);
                }
                else {
                    lcd->setCursor(6, 0);
                    lcd->print("0000");
                }

                if (gv_profget.start_min < 10)
                    lcd->setCursor(9, 0);
                else
                    lcd->setCursor(8, 0);
                lcd->print(gv_profget.start_min);
            }

            if (gv_short_sw) {
                nextSC4(SC4ETIME);
            }
            break;
        case SC4ETIME:
            blinkCursor(4, 1, true);

            if (gv_rot_active) {
                gv_rot_active = false;
                if (gv_rot_pos) {
                    gv_rot_pos = false;
                    gv_profget.duration++;
                }
                if (gv_rot_neg) {
                    gv_rot_neg = false;
                    gv_profget.duration--;
                }

                if (gv_profget.duration > 23) {
                    gv_profget.duration = 0;
                }

                if (gv_profget.duration < 0) {
                    gv_profget.duration = 23;
                }

                lcd->setCursor(6, 1);
                lcd->print("00");

                if (gv_profget.duration < 10)
                    lcd->setCursor(7, 1);
                else
                    lcd->setCursor(6, 1);
                lcd->print(gv_profget.duration);
            }

            if (gv_short_sw) {
                nextSC4(SC4TEMP);
            }
            break;
        case SC4TEMP:
            blinkCursor(11, 0, true);

            if (gv_rot_active) {
                gv_rot_active = false;
                if (gv_rot_pos) {
                    gv_rot_pos = false;
                    gv_profget.tset++;
                }
                if (gv_rot_neg) {
                    gv_rot_neg = false;
                    gv_profget.tset--;
                }

                if (gv_profget.tset > 99) {
                    gv_profget.tset = 0;
                }

                if (gv_profget.tset < 0) {
                    gv_profget.tset = 99;
                }

                lcd->setCursor(13, 0);
                lcd->print(convrange(gv_profget.tset));
            }

            if (gv_short_sw) {
                nextSC4(SC4CSET);
            }
            break;
        case SC4CSET:
            blinkCursor(11, 1, true);

            if (gv_rot_active) {
                gv_rot_active = false;
                if (gv_rot_pos) {
                    gv_rot_pos = false;
                    if (gv_curr_scale == SCALE_F)
                        gv_profget.hystrange += 0.59;
                    else
                        gv_profget.hystrange += 1;
                }
                if (gv_rot_neg) {
                    gv_rot_neg = false;
                    if (gv_curr_scale == SCALE_F)
                        gv_profget.hystrange -= 0.59;
                    else
                        gv_profget.hystrange -= 1;
                }

                if (gv_profget.hystrange > 9)
                    gv_profget.hystrange = 0;
                if (gv_profget.hystrange < 0)
                    gv_profget.hystrange = 9;

                lcd->setCursor(13,1);
                lcd->print(convrange(gv_profget.hystrange));
            }

            if (gv_short_sw) {
                nextSC4(SC4SAVE);
            }
            break;

        case SC4SAVE:
            gv_profile[gv_prof_get].op_mode   = gv_profget.op_mode;
            gv_profile[gv_prof_get].start_hr  = gv_profget.start_hr;
            gv_profile[gv_prof_get].start_min = gv_profget.start_min;
            gv_profile[gv_prof_get].duration  = gv_profget.duration;
            gv_profile[gv_prof_get].tset      = gv_profget.tset;
            gv_profile[gv_prof_get].hystrange = gv_profget.hystrange;

            lcd->clear();
            lcd->setCursor(0,0);
            saveProf(gv_prof_get);      // Save the modified data

            lcd->print("Profile");      // Message for user
            showProf(8, 0);
            lcd->setCursor(10,0);
            lcd->print("saved");

            gv_inputchange = true;      // Set input change flag
            delay(1000);

            if (gv_prof_get > PROF_OFF)
                Screen4(SHOW_PSET);     // Refresh display
            else
                Screen4(SHOW_PGET);

            nextSC4(SC4NULL);
            break;
    }
}


void getScreen5()
{
    switch(gv_scr5state) {
        case SC5NULL:
            if (gv_short_sw) {
                nextSC5(SC5YEAR);
            }
            break;
        case SC5YEAR:
            blinkCursor(0, 0, true);

            if (gv_rot_active) {
                gv_rot_active = false;
                if (gv_rot_pos) {
                    gv_rot_pos = false;
                    gv_year_val++;
                }
                if (gv_rot_neg) {
                    gv_rot_neg = false;
                    gv_year_val--;
                }

                if (gv_year_val > 2099) {
                    gv_year_val = 2015;
                }

                if (gv_year_val < 2015) {
                    gv_year_val = 2099;
                }
            }

            lcd->setCursor(2,0);
            lcd->print(gv_year_val);
            lcd->setCursor(0,0);

            if (gv_short_sw) {
                nextSC5(SC5MON);
            }
            break;
        case SC5MON:
            blinkCursor(7, 0, true);

            if (gv_rot_active) {
                gv_rot_active = false;
                if (gv_rot_pos) {
                    gv_rot_pos = false;
                    gv_mon_num++;
                }
                if (gv_rot_neg) {
                    gv_rot_neg = false;
                    gv_mon_num--;
                }

                if (gv_mon_num > 12) {
                    gv_mon_num = 1;
                }

                if (gv_mon_num < 1) {
                    gv_mon_num = 12;
                }
            }

            lcd->setCursor(9,0);
            lcd->print(gv_mon_num);
            lcd->setCursor(7,0);

            if (gv_short_sw) {
                nextSC5(SC5DAY);
            }
            break;
        case SC5DAY:
            lcd->setCursor(12,0);
            blinkCursor(12, 0, true);

            if (gv_rot_active) {
                gv_rot_active = false;
                if (gv_rot_pos) {
                    gv_rot_pos = false;
                    gv_day_val++;
                }
                if (gv_rot_neg) {
                    gv_rot_neg = false;
                    gv_day_val--;
                }

                if (gv_day_val > 31) {
                    gv_day_val = 1;
                }

                if (gv_day_val < 1) {
                    gv_day_val = 31;
                }
            }

            lcd->setCursor(14,0);
            lcd->print(gv_day_val);
            lcd->setCursor(12,0);

            if (gv_short_sw) {
                nextSC5(SC5HOUR);
            }
            break;
        case SC5HOUR:
            blinkCursor(0, 1, true);

            if (gv_rot_active) {
                gv_rot_active = false;
                if (gv_rot_pos) {
                    gv_rot_pos = false;
                    gv_currhour++;
                }
                if (gv_rot_neg) {
                    gv_rot_neg = false;
                    gv_currhour--;
                }

                if (gv_currhour > 23) {
                    gv_currhour = 0;
                }

                if (gv_currhour < 0) {
                    gv_currhour = 23;
                }
            }

            lcd->setCursor(2,1);
            lcd->print("00");
            if (gv_currhour < 10)
                lcd->setCursor(3,1);
            else
                lcd->setCursor(2,1);
            lcd->print(gv_currhour);
            lcd->setCursor(0,1);

            if (gv_short_sw) {
                nextSC5(SC5MIN);
            }
            break;
        case SC5MIN:
            blinkCursor(5, 1, true);

            if (gv_rot_active) {
                gv_rot_active = false;
                if (gv_rot_pos) {
                    gv_rot_pos = false;
                    gv_currmin++;
                }
                if (gv_rot_neg) {
                    gv_rot_neg = false;
                    gv_currmin--;
                }

                if (gv_currmin > 59) {
                    gv_currmin = 0;
                }

                if (gv_currmin < 0) {
                    gv_currmin = 59;
                }
            }

            lcd->setCursor(7,1);
            lcd->print("00");
            if (gv_currmin < 10)
                lcd->setCursor(8,1);
            else
                lcd->setCursor(7,1);
            lcd->print(gv_currmin);
            lcd->setCursor(5,1);

            if (gv_short_sw) {
                nextSC5(SC5SET);
            }
            break;
        case SC5SET:
            lcd->clear();
            lcd->setCursor(0,0);
            // Set the RTC with new time/day values
            if (setTstatTime())
                lcd->print("Clock set OK");
            else
                lcd->print("Clock set failed");
            delay(2000);
            Screen5();

            nextSC5(SC5NULL);
            break;
    }
}
