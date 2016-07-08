//-------------------------------------------------------------------
// tstat_lcd.h
//
// Created for "Arduino: A Technical Reference", 2015, J. M. Hughes
// Chapter 12
//-------------------------------------------------------------------

#ifndef TSTATLCD_H
#define TSTATLCD_H

void TitleDisp(char *msg1, char *msg2, int dtime);

void Screen1();
void Screen2();
void Screen3();
void Screen4();
void Screen5();

void blinkCursor(int row, int col, bool mode);

#endif
