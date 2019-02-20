/* 
 * File:   AD5440_drvr.h
 * Author: Marshall
 *
 * Created on August 26, 2018, 5:46 PM
 */

#ifndef AD5440_DRVR_H
#define	AD5440_DRVR_H

//Global variables
static int AUXMidscale[2] = {0x01FF, 0x01FF};  //Midscale output for Aux DACs

void write_aux_a(int& value);
void write_aux_b(int& value);

#endif	/* AD5440_DRVR_H */

