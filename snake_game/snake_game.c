/*
 * snake_game.c
 *
 * Created: 26/10/2012 14:18:17
 *  Author: flavio
 * update: 30/10/2012 16:00 - save max score in eeprom
 * update: 30/10/2012 16:30 - pause button
 * update: bug fixed: direction can't be changed 2 times in just one round
 */ 

      #include "snake_game.h"
	  #include "74HC595.h"
   
	  //Led Dot Matrix Pin information (Common Anode)
	  //================================================   
	  /* columns correspondent bits
       int cols[8] = {13, 3, 4, 10, 6, 11, 15, 16}; //must be clear to light the led

       // rows correspondent bits
       int rows[8] = {9, 14, 8, 12, 1, 7, 2, 5}; //must be set to light the led*/
	   //===============================================
	
		
		static void ioinit();   
	  
	   // Global variables
	   //===============================================
		unsigned char direction = LEFT;
		unsigned char screen[NUM_ROWS] = CLEARSCREEN;
		struct snake_cell *pHead;
		uint8_t fruit_row;
		uint8_t fruit_col;
		uint16_t score = 0;
		unsigned char size = 4;
		//==============================================
		#define NUM_SIMBOLS  21                                                                                                                                                                                                                                                       // :                     //P                   //L                   //A                //C                      //R                                                       //M                         //E                  //X                    //S      
	    const unsigned char table[NUM_SIMBOLS][8] PROGMEM = {{24,36,36,36,36,36,24,0},{16,24,20,16,16,16,16,0},{60,32,32,60,4,4,60,0},{60,32,32,56,32,32,60,0},{36,36,36,60,32,32,32,0},{60,4,4,60,32,32,60,0}, {16,8,4,28,36,36,24,0},{60,36,32,16,8,8,8,0},{60,36,36,60,36,36,60,0},{60,36,36,60,32,32,60,0},{0,0,24,24,0,24,24,0},{60,36,36,60,4,4,4,0},{4,4,4,4,4,4,60,0},{60,36,36,60,36,36,36,0},{60,4,4,4,4,4,60,0},{60,36,36,60,20,36,36,0},CLEARSCREEN,{99,85,73,65,65,65,65,0},{60,4,4,60,4,4,60,0},{66,36,24,24,36,66,129,0},{56,4,4,24,32,32,28,0}};
	                                                                                                                                                                             //{60,4,4,60,36,36,60,0}                                                                                                                                                                                                                                                                              
		//Main Function
		//================================================================				   
	   int main (void)
	   {
		   unsigned long count = 0;
		   //put EEPROM data to this variable (max score stored)
		   volatile uint8_t EEByte;
		   //uint8_t letras[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,17,18,19,20};
			   
		   ioinit(); //Setup IO pins and defaults
		   //slidePattern(letras,16,1000);
		   
		   //read from EEPROM
		   EEByte = eeprom_read_byte((uint8_t*)EEADDR_MAXSCORE);
		   if(EEByte == 0xFF){//if EEPROM was not setted yet, set to zero
			   //write EEPROM
			   eeprom_write_byte ((uint8_t*)EEADDR_MAXSCORE, 180);
		    }
		   face();
		   
		   pHead = create_snake(size,4,4,direction);
		   create_fruit(count,pHead);
		   
		   while(1)
		   {
			   count++;
			   if(count == 4294967294){
				    count = 0;   
			   }
					draw_snake(screen,pHead,fruit_row,fruit_col);
					#if F_CPU == 1000000UL
						display_pixels(screen,SLIDE_DELAY_SCREEN + SLIDE_DELAY_SCREEN/4 - (score/2)); //accelerates according to the score 
					#else
						#if F_CPU == 16000000UL
							display_pixels(screen,SLIDE_DELAY_SCREEN + SLIDE_DELAY_SCREEN/4 - (score*5)); //accelerates according to the score
						#endif
					#endif
					move(direction,pHead,count);
		   }                 //          | 
	   }//end main function
	   //=========================================================================
	   
	   void ioinit (void)
	   {
		   DDRC  = 0b00000111; //1 = output, 0 = input
		   PORTC = 0b00000000;
		   DDRB  = 0; //all pins input
		   PORTB = 0xFF;
		   DDRD = 0;
		   PORTD = 0xFF;
	   }

       //function to print the screen
		void display_pixels(unsigned char screen[8],int delay){
			unsigned int pattern;
			uint8_t direction_changed = NO;
			
			for(int i =0 ; i< delay; i++){
				if(bit_is_clear(BUTTON_UP_PORT,BUTTON_UP_PIN)){
					if((direction != DOWN) && (direction_changed == NO)){
					//if((direction != DOWN)){
						direction = UP;
						direction_changed = YES;
					}
				}
				if(bit_is_clear(BUTTON_LEFT_PORT,BUTTON_LEFT_PIN)){
					if((direction != RIGHT) && (direction_changed == NO)){
						direction = LEFT;
						direction_changed = YES;
					}
				}
				if(bit_is_clear(BUTTON_DOWN_PORT,BUTTON_DOWN_PIN)){
					if((direction != UP) && (direction_changed == NO)){
						direction = DOWN;
						direction_changed = YES;
					}
				}
				if(bit_is_clear(BUTTON_RIGHT_PORT,BUTTON_RIGHT_PIN)){
					if((direction != LEFT) && (direction_changed == NO)){
						direction = RIGHT;
						direction_changed = YES;
					}
				}
				if(bit_is_clear(BUTTON_PAUSE_PORT,BUTTON_PAUSE_PIN)){
					_delay_ms(300);
					while(bit_is_set(BUTTON_PAUSE_PORT,BUTTON_PAUSE_PIN));
					_delay_ms(300);
				}
				for(int row = 0; row < NUM_ROWS; row++){
					pattern =  ~screen[row];    //draw columns
					pattern <<= NUM_COLS;
					pattern |= 0b00000001<<row; // row by row individualy
					output_led_state(pattern,MSBFIRST);
				}
			}
		}					
		void on(uint8_t row, uint8_t col){
			screen[row] |= _BV(col);
		}
		void off(unsigned char screen[NUM_ROWS], uint8_t row, uint8_t col){
			screen[row] &= ~_BV(col);
		}
		void move(unsigned char direction,struct snake_cell *pHead,unsigned long count){
			    uint8_t row,col, prev_row, prev_col;
				struct snake_cell *pAux, *pAux2;
				uint8_t growup = NO;
				unsigned char i;
				
				row = pHead->row;
				col = pHead->col;
				if(direction == UP){
					if(row == 0){
					     //colision with wall
						 #ifndef __EASY__
							game_over();
						 #endif
						 //or start on the other side
						 row = NUM_ROWS-1;	
					}else{
						row--;
					}	
				}else{
					if(direction == DOWN){
						   if(row == NUM_ROWS-1){
							  //colision with wall
						      #ifndef __EASY__
							      game_over();
						      #endif

							 //or start on the other side
							 row = 0;   
						   }else{
							   row++;
						   }
					}else{
						 if(direction == LEFT){
						      if(col == 0){
									//colision with wall
								    #ifndef __EASY__
									    game_over();
								    #endif
	
									//or start on the other side
									col = NUM_COLS - 1;  
							   }else{
								  col--;   
							   }								   	 
						 }else{
							 if(direction == RIGHT){
									if(col == NUM_COLS - 1){
										//colision with wall
										#ifndef __EASY__
											game_over();
										#endif

										//or start on the other side
										col = 0;	
									}else{
										col++;	
									}																				 
							  }								 	 
						 }
					}					
				}
				//check colision
				i = size; //i = size of the snake
				pAux = pHead;
				while(i--){
					if((pAux->row == row) && (pAux->col == col)){
						#ifndef __EASY__
							game_over();
						#endif
					}
					pAux = pAux->next;
				}
								
				// check if the snake eat the fruit
				if((row == fruit_row) && (col == fruit_col)){ 
				      	growup = YES;
						score += FRUIT_VALUE; //if yes, give the prize
						size++;
				}
				
				//move the snake_cell's coordenates forward
				prev_row = pHead->row;
				prev_col = pHead->col;
				pHead->row = row;
				pHead->col = col;
				row = prev_row;
				col = prev_col;
				pAux = pHead->next;
				while(pAux != NULL){
					prev_row = pAux->row;
					prev_col = pAux->col;
					pAux->row = row;
					pAux->col = col;
					row = prev_row;
					col = prev_col;
					pAux = pAux->next;
					if(pAux->next == NULL){
						if(growup == YES){ //increase 1 snake_cell
							prev_row = pAux->row;
							prev_col = pAux->col;
							pAux->row = row;
							pAux->col = col;
						   pAux2 = (struct snake_cell *) malloc(sizeof(struct snake_cell));
						   pAux2->row = prev_row;
						   pAux2->col = prev_col;
						   pAux2->next = NULL;
						   pAux->next = pAux2;
						   create_fruit(count,pHead);
						   break;
						}
					}	
				}
		}//end move function
		struct snake_cell* create_snake(uint8_t size,uint8_t head_row,uint8_t head_col, unsigned char direction){
			struct snake_cell *pHead;
			struct snake_cell *pAux, *pAux2;
			pHead = (struct snake_cell*) malloc(sizeof(struct snake_cell));
			pHead->row = head_row;
			pHead->col = head_col;
			pHead->next = NULL;
			size--;
			pAux = pHead;
			while(size--){
			    	pAux2 = (struct snake_cell*) malloc(sizeof(struct snake_cell));
					if(direction == LEFT){
					      	pAux2->row = head_row;
							  head_col = head_col == NUM_COLS - 1 ? 0 : head_col + 1;
							pAux2->col = head_col;
					}
					if(direction == RIGHT){
						pAux2->row = head_row;
						head_col = head_col == 0 ? NUM_COLS-1 :head_col - 1;
						pAux2->col = head_col;
					}
					if(direction == UP){
						 head_row = head_row == 0 ? NUM_ROWS - 1 : head_row - 1;
						pAux2->col = head_col;
						pAux2->row = head_row;
					}
					if(direction == DOWN){
						head_row = head_row == NUM_ROWS - 1? 0 :head_row + 1;
						pAux2->col = head_col;
						pAux2->row = head_row;
					}
					pAux->next = pAux2;
					pAux = pAux->next;
					pAux->next = NULL; 
			}
			return pHead;
		}
        void draw_snake(unsigned char screen[NUM_ROWS],struct snake_cell *pHead, uint8_t fruit_row, uint8_t fruit_col){
			struct snake_cell *pAux = pHead;

			clear_screen();
			while(pAux != NULL){
					on(pAux->row,pAux->col);
					pAux = pAux->next;
			}
			on(fruit_row,fruit_col);
		}
		void create_fruit(long count,struct snake_cell *pHead){
			uint8_t row,col;
			struct snake_cell *pAux;
			row = count%8;
			count++;
			count++;
			col = count%8;
			pAux = pHead;
			do{
				if((col == pAux->col) && (row == pAux->row)){
				      	count++;
						row = rand()%8;
						count++;
						count++;
						col = rand()%8;
						pAux = pHead;
						break;
				}
			}while(++pAux);
			fruit_row = row;
			fruit_col = col;
		}
		
		void face()
		{
			for(uint8_t j = 0; j<2; j++){
				clear_screen();
				//generate the face
				//left eye
				on(0, 0);
				on(0, 1);
				on(1, 0);
				on(1, 1);
				//right eye
				on(0, 6);
				on(0, 7);
				on(1, 6);
				on(1, 7);
				//nose
				on(3, 3);
				on(3, 4);
				on(4, 3);
				on(4, 4);
				//mouth
				on(5, 0);
				on(5, 7);
				on(6, 1);
				on(6, 6);
				on(7, 2);
				on(7, 3);
				on(7, 4);
				on(7, 5);
				//draw the screen
				display_pixels(screen,SLIDE_DELAY_SCREEN);
				clear_screen();
				display_pixels(screen,SLIDE_DELAY_SCREEN);

			}			
		}  //end face function
		
		void clear_screen(){
			unsigned char   clearscreen[NUM_ROWS] = CLEARSCREEN;
			//clear the screen
			for(uint8_t i =0 ; i < NUM_ROWS; i++){
				screen[i] = clearscreen[i];
			}
		}
		void game_over(){
			unsigned char x[NUM_ROWS] = X;
			uint8_t count = 3;
			//put EEPROM data to this variable (max score stored)
			volatile uint8_t maxscore;
			
			//read from EEPROM
			maxscore = eeprom_read_byte((uint8_t*)EEADDR_MAXSCORE);
			if(score > maxscore){
				//write EEPROM
				eeprom_write_byte ((uint8_t*)EEADDR_MAXSCORE, score);
			}
			while(count--){
				//print X on the screen
				for(uint8_t i=0; i< NUM_ROWS; i++){
					screen[i] = x[i];
				}
				display_pixels(screen,SLIDE_DELAY_SCREEN);
				clear_screen();
				display_pixels(screen,SLIDE_DELAY_SCREEN);
			}	
			while(1){
				show_score();
				show_maxscore();
			}		
		}
		void show_score(){              //    P  L  A  C   A  R  :                 
		      uint8_t score_message[8] = {16,11,12,13,14,13,15,10};
			  uint8_t score_number[4];
			  uint8_t score_size,aux;
		      
			  //show placar messsage
			  slidePattern(score_message,8,SLIDE_DELAY_MESSAGE);
			  if(score > 9){
				  if(score > 99){
					score_size = 3;  
				  }else{
					  score_size = 2;
				  }
			  }else{
				  score_size = 1;
			  }		
			          //clear screen
			          score_number[0] = 16;
					  
				      switch(score_size){
						  case 1:
						    score_number[1] = score;
							break;
						  case 2:
						    score_number[1] = score/10;
							score_number[2] = score%10;
							break;
						  case 3:
						    score_number[1] = score/100;
							score_number[2] = (score%100)/10;
							score_number[3] = (score%100)%10;
							break;  
					  }	
			  //show placa number					  
			  slidePattern(score_number,score_size+1,SLIDE_DELAY_MESSAGE*2);
		}
		void show_maxscore(){              //     M  A  X      S  C  0 R  E   :
				uint8_t maxscore_message[11] = {16,17,13,19,16,20,14,0,15,18,10};
				uint8_t maxscore_number[4];
				uint8_t maxscore_size;
				//put EEPROM data to this variable (max score stored)
				volatile uint8_t maxscore = eeprom_read_byte((uint8_t*)EEADDR_MAXSCORE);;
	
				//show max score messsage
				slidePattern(maxscore_message,11,SLIDE_DELAY_MESSAGE);
				if(maxscore > 9){
					if(maxscore > 99){
						maxscore_size = 3;
					}else{
						maxscore_size = 2;
					}
				}else{
					maxscore_size = 1;
				}
				//clear screen
				maxscore_number[0] = 16;
	
				switch(maxscore_size){
					case 1:
						maxscore_number[1] = maxscore;
						break;
					case 2:
						maxscore_number[1] = maxscore/10;
						maxscore_number[2] = maxscore%10;
						break;
					case 3:
						maxscore_number[1] = maxscore/100;
						maxscore_number[2] = (maxscore%100)/10;
						maxscore_number[3] = (maxscore%100)%10;
						break;
				}
				//show placar number
				slidePattern(maxscore_number,maxscore_size+1,SLIDE_DELAY_MESSAGE*2);
		}
		             
		void slidePattern(uint8_t message[], int message_size, int __delay) {
		unsigned char next_pattern[8];
			for(int idx = 0; idx < message_size; idx++){
				//read the first pattern
				for(uint8_t i = 0; i< NUM_ROWS; i++){
					screen[i] = pgm_read_byte(&table[message[idx]][i]);
				}
				//read the next pattern
				if(idx < message_size - 1){
					for(uint8_t i = 0; i< NUM_ROWS; i++){
						next_pattern[i] = pgm_read_byte(&table[message[idx+1]][i]);
					}
				}
				for(uint8_t j = 0; j <= NUM_COLS; j++){
					for(uint8_t i =0 ; i< NUM_ROWS; i++){
						//rotate the screen 1 column
						screen[i] >>= 1;
						if((idx < message_size - 1) && (j>0)){
							//test if the bit is on in the next pattern col is set
							if(next_pattern[i] & (1<<(j-1))){
									//set the bit
									screen[i] |= (1<<(NUM_COLS-1));
								}else{
									//clear the bit
									screen[i] &= ~(1<<(NUM_COLS-1));
								}
							}
						}
						display_pixels(screen,__delay);
					}      		
			}
							
		}// end slide_pattern function
		

