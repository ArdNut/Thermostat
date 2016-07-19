//-------------------------------------------------------------------
// tstat_ctrl.h
//
// Created for "Arduino: A Technical Reference", 2015, J. M. Hughes
// Chapter 12
//-------------------------------------------------------------------

#ifndef TSTATCTRL_H
#define TSTATCTRL_H

int convrange(double rangeC);
int convtemp(double tempC);

void GetTemps();

void setRelay(int ry, bool state);

void CheckProfile();
void SystemControl();

#endif
