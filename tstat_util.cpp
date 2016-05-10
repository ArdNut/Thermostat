//-------------------------------------------------------------------
// tstat_util.cpp
//
// Created for "Arduino in a Nutshell", 2015, J. M. Hughes
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


void RTCUpdate()
{
    curr_time = hour();
    year_val = year();
    day_val = day();

    switch (month()) {
        case 1:
            mon_val[0] = 'J';
            mon_val[1] = 'A';
            mon_val[2] = 'N';
            break;
        case 2:
            mon_val[0] = 'F';
            mon_val[1] = 'E';
            mon_val[2] = 'B';
            break;
        case 3:
            mon_val[0] = 'M';
            mon_val[1] = 'A';
            mon_val[2] = 'R';
            break;
        case 4:
            mon_val[0] = 'A';
            mon_val[1] = 'P';
            mon_val[2] = 'R';
            break;
        case 5:
            mon_val[0] = 'M';
            mon_val[1] = 'A';
            mon_val[2] = 'Y';
            break;
        case 6:
            mon_val[0] = 'J';
            mon_val[1] = 'U';
            mon_val[2] = 'N';
            break;
        case 7:
            mon_val[0] = 'J';
            mon_val[1] = 'U';
            mon_val[2] = 'L';
            break;
        case 8:
            mon_val[0] = 'A';
            mon_val[1] = 'U';
            mon_val[2] = 'G';
            break;
        case 9:
            mon_val[0] = 'S';
            mon_val[1] = 'E';
            mon_val[2] = 'P';
            break;
        case 10:
            mon_val[0] = 'O';
            mon_val[1] = 'C';
            mon_val[2] = 'T';
            break;
        case 11:
            mon_val[0] = 'N';
            mon_val[1] = 'O';
            mon_val[2] = 'V';
            break;
        case 12:
            mon_val[0] = 'D';
            mon_val[1] = 'E';
            mon_val[2] = 'C';
            break;
        default:
            mon_val[0] = 'E';
            mon_val[1] = 'R';
            mon_val[2] = 'R';
            break;
    }
}
