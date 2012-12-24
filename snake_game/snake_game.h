/*
 * snake_game.h
 *
 * Created: 26/10/2012 14:18:47
 *  Author: flavio
 */ 


#ifndef SNAKE_GAME_H_
#define SNAKE_GAME_H_

	#define F_CPU 16000000UL
	//#define F_CPU 1000000UL

	   #include <avr/io.h>
	   #include <util/delay.h>
	   #include <stdlib.h>
	   #include <avr/pgmspace.h>
	   #include <avr/eeprom.h>
	   
	   //use last EEPROM cell to save the max score
	   #define EEADDR_MAXSCORE 0x1FF
	   
	   #define NUM_COLS 8
	   #define NUM_ROWS 8
	   
	   #define NO	    0
	   #define YES      1
	   
	   #define FRUIT_VALUE 5
	   
	   //Buttons setup
	   #define BUTTON_LEFT_PORT		PINB
	   #define BUTTON_LEFT_PIN		PB5   //2
	   #define BUTTON_DOWN_PIN		PB1
	   #define BUTTON_RIGHT_PORT	PINB
	   #define BUTTON_DOWN_PORT		PINB
	   #define BUTTON_RIGHT_PIN		PB2   //3
	   #define BUTTON_UP_PORT		PINB
	   #define BUTTON_UP_PIN		PB3  //0
	   #define BUTTON_PAUSE_PORT	PINB
	   #define BUTTON_PAUSE_PIN		PB4
   
   #define CLEARSCREEN	{0,0,0,0,0,0,0,0}
   #define X			{129,66,36,24,24,36,66,129}
   #define UP 0
   #define DOWN 1
   #define LEFT 2
   #define RIGHT 3
   #define STOP  4
   
   #if F_CPU == 1000000UL
		#define SLIDE_DELAY_MESSAGE 6      // at 1 MHz
		#define SLIDE_DELAY_SCREEN  62     // at 1MHz
	#else
		#if F_CPU == 16000000UL
			#define SLIDE_DELAY_MESSAGE 100  // at 16MHz
			#define SLIDE_DELAY_SCREEN  1000   //at 16MHz
		#endif
   #endif
   
   struct snake_cell{
	   int row;
	   int col;
	   struct snake_cell *next;
   };
   
	  //Led Dot Matrix Pin information (Common Anode)
	  //================================================   
	  /* columns correspondent bits
       int cols[8] = {13, 3, 4, 10, 6, 11, 15, 16}; //must be clear to light the led

       // rows correspondent bits
       int rows[8] = {9, 14, 8, 12, 1, 7, 2, 5}; //must be set to light the led*/
	   //===============================================
	   
	   
	   
	   //Define functions
	   //===============================================
	   void display_pixels(unsigned char screen[8],int delay);
	   void on(uint8_t row, uint8_t col);
	   void off(unsigned char screen[NUM_ROWS], uint8_t row, uint8_t col);
	   void move(unsigned char direction,struct snake_cell *pHead, unsigned long count);
	   struct snake_cell* create_snake(uint8_t size,uint8_t head_row,uint8_t head_col, unsigned char direction);
	   void draw_snake(unsigned char screen[NUM_ROWS],struct snake_cell *pHead, uint8_t fruit_row, uint8_t fruit_col);
	   void create_fruit(long count, struct snake_cell *pHead);
	   void face();
	   void clear_screen();
	   void game_over();
	   void show_score();
	   void show_maxscore();
	   void slidePattern(uint8_t message[], int message_size, int __delay);
	   //===============================================



#endif /* SNAKE_GAME_H_ */