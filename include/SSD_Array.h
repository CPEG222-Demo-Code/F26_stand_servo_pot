//***********************************************************
//  SSD_Array.h
//  Function prototypes for the 4-digit SSD array (SSD_Array.c)
//***********************************************************
#ifndef SSD_ARRAY_H
#define SSD_ARRAY_H
void SSD_init(void); // Initialize GPIO pins for the SSD array
void SSD_update(int digitSelect, int value, int decimalPoint); // Show one digit of value
#endif
