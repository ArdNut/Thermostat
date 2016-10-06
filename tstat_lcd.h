//-------------------------------------------------------------------
// tstat_lcd.h
//
// Created for "Arduino: A Technical Reference", 2015, J. M. Hughes
// Chapter 12
//-------------------------------------------------------------------

#ifndef TSTATLCD_H
#define TSTATLCD_H

void displayActive();

void showMode(int col, int row);
void showProf(int col, int row);

void TitleDisp(char *msg1, char *msg2, int dtime);

void Screen1(bool showadj);
void Screen2();
void Screen3(int mode);
void Screen4(int mode);
void Screen5();

void blinkCursor(int col, int row, bool mode);

#endif
