/*
 * _74HC595.h
 *
 * Created: 26/10/2012 14:19:47
 *  Author: flavio
 */ 


#ifndef __74HC595_H_
#define __74HC595_H_

#include <avr/io.h>


#define DS_PORT    PORTC
#define DS_PIN     0
#define ST_CP_PORT PORTC
#define ST_CP_PIN  1
#define SH_CP_PORT PORTC
#define SH_CP_PIN  2

#define DS_low()  DS_PORT&=~_BV(DS_PIN)
#define DS_high() DS_PORT|=_BV(DS_PIN)
#define ST_CP_low()  ST_CP_PORT&=~_BV(ST_CP_PIN)
#define ST_CP_high() ST_CP_PORT|=_BV(ST_CP_PIN)
#define SH_CP_low()  SH_CP_PORT&=~_BV(SH_CP_PIN)
#define SH_CP_high() SH_CP_PORT|=_BV(SH_CP_PIN)
#define MSBFIRST 1
#define LSBFIRST 0

//Define functions
//==================================================================
void output_led_state(unsigned int __led_state, uint8_t direction);
//==================================================================

#endif /* __74HC595_H_ */