//-------------------------------------------------------------------
// tstat_util.cpp
//
// Created for "Arduino: A Technical Reference", 2015, J. M. Hughes
// Chapter 12
//-------------------------------------------------------------------

#include "Arduino.h"
#include <Time.h>
#include <DS1302RTC.h>

#include "tstat.h"
#include "tstat_gv.h"
#include "tstat_util.h"


int GetEndTime()
{
    return 0;
}


// Get current date and time from the RTC module
void RTCUpdate()
{
    gv_curr_time = hour();
    gv_year_val = year();
    gv_day_val = day();

    switch (month()) {
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
