/* 
 * File:   Simple_SAIM.h
 * Author: Marshall
 *
 * Created on August 26, 2018, 6:02 PM
 */

#ifndef SIMPLE_SAIM_H
#define SIMPLE_SAIM_H


//Global variables
const int maxSteps = 128;  //Maximum size of the experiment
int stepListX[maxSteps];  //List of positions to step through along X
int stepListY[maxSteps];  //List of positions to step through along Y
static int currStep;      //Current step number
static int steps;         //Number of steps in the experiment
static int8 direction;     //Direction to step in

int create_simple_half(int);
int create_simple_full(int, int, int);
void start_simple(void);
void start_dithered(int*);
void stop_simple(void);

#endif	/* SIMPLE_SAIM_H */

