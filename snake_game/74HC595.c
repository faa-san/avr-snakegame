/*
 * _74HC595.c
 *
 * Created: 26/10/2012 14:19:17
 *  Author: flavio
 */ 
#include "74HC595.h"


	   // function that shift the bytes to two 74hc595's
	   void output_led_state(unsigned int __led_state, uint8_t direction)
	   {
		   SH_CP_low();
		   ST_CP_low();
		   if(direction == LSBFIRST){
			   for (int i=0;i<16;i++)
			   {
				   if ((_BV(i) & __led_state) == _BV(i))  //bit_is_set doesn’t work on unsigned int so we do this instead
				   DS_high();
				   else
				   DS_low();
				   
				   
				   SH_CP_high();
				   SH_CP_low();
			   }
		   }else{
			   for (int i=15;i>=0;i--)
			   {
				   if ((_BV(i) & __led_state) == _BV(i))  //bit_is_set doesn’t work on unsigned int so we do this instead
				   DS_high();
				   else
				   DS_low();
				   
				   
				   SH_CP_high();
				   SH_CP_low();
			   }
		   }
		   ST_CP_high();
	   }