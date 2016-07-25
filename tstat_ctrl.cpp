//-------------------------------------------------------------------
// tstat_ctrl.cpp
//
// Created for "Arduino: A Technical Reference", 2015, J. M. Hughes
// Chapter 12
//-------------------------------------------------------------------
#include <DHT22.h>
#include "tstat_gv.h"

int convrange(double rangeC)
{
    if (gv_curr_scale == SCALE_F) {
        return int(1.8 * rangeC);
    }
    else
        return int(rangeC);
}

int convtemp(double tempC)
{
    if (gv_curr_scale == SCALE_F) {
        return int(1.8 * tempC + 32);
    }
    else
        return int(tempC);
}


// Read temp and humidity from internal and external sensors (if
// external sensor is present)
void GetTemps()
{
    // read internal DHT22 sensor
    gv_dhtchk = DHT->readData();

    // Only update temp and humidity if no read error
    if (gv_dhtchk == DHT_ERROR_NONE) {
        gv_tint_last = gv_tint_curr;
        gv_tint_curr = (DHT->getTemperatureCInt()/10) + gv_temp_adj;

        gv_hint_last = gv_hint_curr;
        gv_hint_curr = (DHT->getHumidityInt()/10) + gv_hum_adj;
    }

    // external sensor goes here

}


void setRelay(int ry, bool state)
{
    switch(ry) {
        case 0:
            if (state) {
                digitalWrite(RY1, LOW);
            }
            else {
                digitalWrite(RY1, HIGH);
            }
            break;
        case 1:
            if (state) {
                digitalWrite(RY2, LOW);
            }
            else {
                digitalWrite(RY2, HIGH);
            }
            break;
        case 2:
            if (state) {
                digitalWrite(RY3, LOW);
            }
            else {
                digitalWrite(RY3, HIGH);
            }
            break;
        case 3:
            if (state) {
                digitalWrite(RY4, LOW);
            }
            else {
                digitalWrite(RY4, HIGH);
            }
            break;
    }
}


void CheckProfile()
{
    int i;

    // only update profile data if input not active
    if (gv_input_active) {
        return;
    }

    // if profiles disabled then set to PROF_OFF values and return
    if (gv_prof_set == PROF_OFF) {
        gv_prof_set = PROF_OFF;
        gv_mode_set = gv_profile[PROF_OFF].op_mode;
        gv_temp_set = gv_profile[PROF_OFF].tset;
        gv_hyst_set = gv_profile[PROF_OFF].hystrange;
        return;
    }

    // Look at the times for the day, nite, and weekend profiles
    for (i = 1; i < PROF_MAX; i++) {
        if (gv_profile[i].start_hr >= gv_currhour &&
            gv_profile[i].start_min >= gv_currmin) {
            if (gv_prof_set != i) {
                gv_prof_set = i;
                gv_mode_set = gv_profile[i].op_mode;
                gv_temp_set = gv_profile[i].tset;
                gv_hyst_set = gv_profile[i].hystrange;
            }
        }
    }
}


// Using current profile data, control the HVAC system
void SystemControl()
{
    // only update control outputs if input not active
    if (gv_input_active) {
        return;
    }

    // if doing nothing, then return immediately
    if (gv_mode_set == MODE_OFF) {
        setRelay(0, false);
        setRelay(1, false);
        setRelay(2, false);
        setRelay(3, false);
        return;
    }
    else {
        // if temp OK but humidity is high, enable fan for override time
        // the parens are redendant, but it helps make things clearer
        //
        // If no profile is active then we use the "PROF_OFF" (index 0)
        // profile data, which is the same as the manual settings
        // obtained from screen 3.
        switch(gv_mode_set) {
            case MODE_COOL:
                    // cooling on
                    if (gv_tint_curr > (gv_profile[gv_prof_set].tset + (gv_profile[gv_prof_set].hystrange/2))) {
                        setRelay(3, true);  // enable common power
                        setRelay(2, true);  // enable fan
                        delay(1000);
                        setRelay(1, true);  // enable cooling
                    }
                    // cooling off
                    else if (gv_tint_curr < (gv_profile[gv_prof_set].tset - (gv_profile[gv_prof_set].hystrange/2))) {
                        setRelay(1, false);  // disable cooling
                        delay(1000);
                        setRelay(2, false);  // disable fan
                        setRelay(3, false);  // disable common power
                    }
                break;
            case MODE_HEAT:
                    // heating on
                    if (gv_tint_curr < (gv_profile[gv_prof_set].tset + (gv_profile[gv_prof_set].hystrange/2))) {
                        setRelay(3, true);  // enable common power
                        setRelay(2, true);  // enable fan
                        delay(1000);
                        setRelay(0, true);  // enable heater
                    }
                    // heating off
                    else if (gv_tint_curr > (gv_profile[gv_prof_set].tset - (gv_profile[gv_prof_set].hystrange/2))) {
                        setRelay(0, false);  // disable heating
                        delay(1000);
                        setRelay(2, false);  // disable fan
                        setRelay(3, false);  // disable common power
                    }
                break;
            case MODE_FAN:
                    setRelay(3, true);  // enable common power
                    setRelay(2, true);  // enable fan
                break;
            case MODE_AUTO:
                    // command cooling
                    if (gv_tint_curr > (gv_profile[gv_prof_set].tset + (gv_profile[gv_prof_set].hystrange/2))) {
                        setRelay(3, true);  // enable common power
                        setRelay(2, true);  // enable fan
                        delay(1000);
                        setRelay(1, true);  // enable cooling
                    }
                    // command heating
                    // adjust heating setpoint down by hystrange amount
                    else if (gv_tint_curr < ((gv_profile[gv_prof_set].tset-gv_profile[gv_prof_set].hystrange) +
                                            (gv_profile[gv_prof_set].hystrange/2))) {
                        setRelay(3, true);  // enable common power
                        setRelay(2, true);  // enable fan
                        delay(1000);
                        setRelay(0, true);  // enable heater
                    }
                    else if ((gv_tint_curr < (gv_profile[gv_prof_set].tset - (gv_profile[gv_prof_set].hystrange/2))) &&
                            (gv_tint_curr > (gv_profile[gv_prof_set].tset - (gv_profile[gv_prof_set].hystrange/2)))) {
                        setRelay(0, false);
                        setRelay(1, false);
                        setRelay(2, false);
                        setRelay(3, false);
                    }
                break;
        }
    }
}
