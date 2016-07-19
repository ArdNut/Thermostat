//-------------------------------------------------------------------
// tstat_util.h
//
// Created for "Arduino: A Technical Reference", 2015, J. M. Hughes
// Chapter 12
//-------------------------------------------------------------------

#ifndef TSTATUTIL_H
#define TSTATUTIL_H

void DispLED(bool state);

int freeRam();

int GetEndTime();
bool setTstatTime();
void RTCUpdate();

bool checkEEPROM();
bool enableEEPROM();

void EEPROM_writeDouble(int ee, double value);
double EEPROM_readDouble(int ee);

void saveGVData();
void readGVData();

void saveProf(uint8_t prof_id);
void readProf(uint8_t prof_id);

void resetGVData();

#endif
