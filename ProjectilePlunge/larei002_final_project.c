/*	Author: Lizbeth Areizaga
 *  Partner(s) Name:
 *	Lab Section:021
 *	Assignment: Project
 *	Exercise Description:
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo 1 URL: https://youtu.be/6xqISCjyJo4
 *	Demo 2 URL: https://youtu.be/O8Q8D7RK79c
 *  Final Demo: https://youtu.be/LOXY49CsT1s
 */
#include <avr/io.h>
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include <util/delay.h>

#include "nokia5110.h"
#endif

//used getBit function from zybooks
unsigned char GetBit(unsigned char x, unsigned char k) {
   return ((x & (0x01 << k)) != 0);
}

void sendToSR(unsigned char data){
  PORTC &= 0xFD; //RCLKL = 0 set C1 to 0

  for(unsigned char i = 0; i < 8; ++i){
    PORTC = (PORTC & 0xFE) | GetBit(data, (i)); //SER gets value of data at certain position
    PORTC |= 0x04; //SRCLK = 1 set C2 to 1
    PORTC &= 0xFB;
  }
  PORTC |= 0x02;
}

//copied from lab manual

void ADC_init(){
                  ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
          }

unsigned long int findGCD(unsigned long int a, unsigned long int b){
	unsigned long int c;
	while(1){
		c = a%b;
		if(c==0){return b;}
		a = b;
		b = c;
	}
	return 0;
}

typedef struct task {
	signed char state;
	unsigned long int period;
	unsigned long int elapsedTime;
	int (*TickFct) (int);
} task;

//global variables
unsigned char led = 0;
unsigned char byte[] = {0x1F, 0x87, 0xF8, 0x83, 0x80, 0xF8};
unsigned char byteSize = 6;
unsigned char j = 0;
unsigned char row = 0x00;
unsigned char on = 0;
unsigned char end = 0;
unsigned char lose = 0x00;
unsigned char display_done = 0;
unsigned char score = 0;
char score_char = '0';

enum Monitor_States {Monitor_Start, Monitor_off, Monitor_onp, Monitor_onr, Monitor_offp};
int MonitorSMTick(int state){
	unsigned char button = ~PINA & 0x04;
	switch(state){
		case Monitor_Start:
			state = Monitor_off;
			on = 0x00;
			sendToSR(0xFF);
      sendToSR(0x00);
			break;

		case Monitor_off:
    nokia_lcd_write_string("Welcome! Press button to start.", 1);
    nokia_lcd_render();
    nokia_lcd_clear();
			if(!button){
				state = Monitor_off;
			} else {
				state = Monitor_onp;
			}
			break;

		case Monitor_onp:
			if(button){
				state = Monitor_onp;
			} else {
				state = Monitor_onr;
        on = 0x01;
				//nokia_lcd_clear();
			}
			break;

		case Monitor_onr:
    nokia_lcd_write_string("Press button to reset.", 1);
    nokia_lcd_render();
    nokia_lcd_clear();
    if(on == 0x00){
      state = Monitor_off;
      sendToSR(0xFF);
      sendToSR(0x00);
    } else {
			if(!button){
				state = Monitor_onr;
			} else {
				state = Monitor_offp;
			}
    }
			break;

		case Monitor_offp:
			if(button){
				state = Monitor_offp;
			} else {
				state = Monitor_off;
        on = 0x00;
				sendToSR(0xFF);
        sendToSR(0x00);
			}
			break;

		default:
			state = Monitor_Start;
			break;

	}
	return state;
}

enum Game_States{Game_Start, Game_wait_start, Game_play, Game_wait_end, Game_reset};
int GameSMTick(int state){
	unsigned  char led_compare = 0x00;
	for(unsigned char k = 0; k < 8; ++k){
		led_compare = (led_compare << 1) | GetBit(led, k);
	}
	switch(state){
		case Game_Start:
			state = Game_wait_start;
			lose = 0x00;
			score = 0;
			break;

		case Game_wait_start:
			if(on == 0x00){
				state = Game_wait_start;
			} else if(on == 0x01){
				state = Game_play;
			}
			break;

		case Game_play:
			if(on == 0x00){
        state = Game_wait_start;
        score = 0;
        lose = 0x00;
      } else if (on == 0x01 && lose == 0x00 && score < 100){
        state = Game_play;
      } else if ((on == 0x01 && lose == 0x01 && score < 100) || (on == 0x01 && lose == 0x00 && score >= 100)){
        state = Game_wait_end;
        end = 0x01;
      }
			break;

		case Game_wait_end:
    if(display_done == 0x00){
      state = Game_wait_end;
    } else if (display_done == 0x01){
      state = Game_reset;
      end = 0x00;
      on = 0x00;
      lose = 0x00;
      score  = 0x00;
    }
		break;

    case Game_reset:
    state = Game_wait_start;
    break;

		default:
			state = Game_Start;
			break;
	}

	switch(state){
		case Game_play:
    if( ((led_compare & (~byte[j]) ) == 0x00) && row == 0x80){
      score += 5;
        if(score <= 0){
          score_char = '0';
        } else if (score <= 10){
          score_char = '1';
        } else if (score <= 20){
          score_char = '2';
        } else if(score <= 30){
          score_char = '3';
        } else if(score <= 40){
          score_char = '4';
        } else if(score <= 50){
          score_char = '5';
        } else if(score <= 60){
         score_char = '6';
       } else if(score <= 70){
         score_char = '7';
       } else if(score <= 80){
         score_char = '8';
       } else if(score <= 90){
         score_char = '9';
       } else if (score > 90){
         score_char = '*';
       }
    } else if(( !((led_compare & (~byte[j]) ) == 0x00) && row == 0x80)){
      lose = 0x01;
    }
		break;

		case Game_reset:
			//score = 0;
      //lose = 0x00;
      //on = 0x00;
			break;

		default:
			break;
	}


	return state;
}

enum Joystick_States {Joystick_start, Joystick_off, Joystick_wait, Joystick_right, Joystick_left};
int JoystickSMTick(int state){
	  unsigned short adc_val = 0;
		adc_val = ADC;
		ADMUX = 1; // for port a1, connected to left and right
		unsigned short x_val = adc_val;

	switch (state){
			case Joystick_start:
			led = 0x00;
			state = Joystick_off;
			break;

      case Joystick_off:
      if(on == 0x00){
        state = Joystick_off;
      } else if (on == 0x01){
        state = Joystick_wait;
        led = 0x01;
      }
      break;

			case Joystick_wait:
			if(x_val > 800 && led < 128){
				state = Joystick_right;
				led *= 2;
			} else if (x_val < 50 && led > 1){
				state = Joystick_right;
				led /= 2;
			}else if (x_val >= 50 && x_val <=800){
				state = Joystick_wait;
				//led = 0x00;
			}
      if(on == 0x00){
        state = Joystick_off;
        led = 0x00;
      }
			break;

			/*case Joystick_left:
      if(x_val > 800 && led < 128){
				state = Joystick_right;
				led *= 2;
			} else if (x_val < 50 && led > 1){
				state = Joystick_right;
				led /= 2;
			}else if (x_val >= 50 && x_val <=800){
				state = Joystick_wait;
				//led = 0x00;
			}
			break;*/

			case Joystick_right:
      if(x_val > 800 && led < 128){
				state = Joystick_right;
				led *= 2;
			} else if (x_val < 50 && led > 1){
				state = Joystick_right;
				led /= 2;
			}else if (x_val >= 50 && x_val <=800){
				state = Joystick_wait;
				//led = 0x00;
			}
      if(on == 0x00){
        state = Joystick_off;
        led = 0x00;
      }
			break;

			default:
			state = Joystick_start;
			break;
	}

return state;
}

enum Matrix_States {Matrix_Start, Matrix_wait, Matrix_scroll};
int MatrixSMTick(int state){
  switch(state){
    case Matrix_Start:
    state = Matrix_wait;
    row = 0x01;
    j = 0;
    break;

    case Matrix_wait:
    if(on == 0x00){
      state = Matrix_wait;
    } else if (on == 0x01){
      state = Matrix_scroll;
      row = 0x01;
      j = 0;
    }
    break;

    case Matrix_scroll:
    if(on == 0x01){
      state = Matrix_scroll;
    } else if (on == 0x00){
      state = Matrix_wait;
    }
    break;

    default:
    state = Matrix_Start;
    break;
  }

  switch (state){
    case Matrix_scroll:
    if ((row & 0x80) == 0x80){
      if(j < byteSize -1 ){
                ++j;
       } else {
	       j = 0;
       }
    }
    if(row < 128){
     row = (row << 1);
    } else {
	    row = 0x01;
    }
    break;

    default:
    break;
  }

  return state;
}

enum Output_States {Output_Start, Output_display_wait, Output_display_game, Output_display_gameover};
int OutputSMTick (int state){
	switch (state){
		case Output_Start:
		state = Output_display_wait;
    PORTD = 0x00;
    display_done = 0x00;
		break;

    case Output_display_wait:
    if(on == 0x00){
      state = Output_display_wait;
    } else if (on == 0x01 && end == 0x00){
      state = Output_display_game;
    }
    //nokia_lcd_write_string("Press button to start.", 1);
    break;

		case Output_display_game:
    if (on == 0x00){
      state = Output_display_wait;
    } else if(end == 0x00 && on == 0x01){
			state = Output_display_game;
		} else if (end == 0x01 && on == 0x01){
			state = Output_display_gameover;
      display_done = 0x00;
		}
		break;

    case Output_display_gameover:
    if(on == 0x00){
      state = Output_display_wait;
      display_done = 0x01;
    } else {
      state = Output_display_gameover;
    }
    break;

		default:
		state = Output_Start;
		break;
	}

  switch(state){

    case Output_display_wait:
    PORTD = 0x00;
    break;

    case Output_display_game:
      PORTD = led;
      sendToSR(~row);
			sendToSR(~byte[j]);
    break;

    case Output_display_gameover:
    nokia_lcd_set_cursor(0, 20);
    nokia_lcd_write_string("Score: ", 1);
    nokia_lcd_write_char(score_char, 1);
    /*for(unsigned char h = 0; h < 8; ++h){
      nokia_lcd_set_cursor(h*4, 30);
      unsigned char curr_bit = (curr_bit& 0x01) | GetBit(score, (7-h));
      if((curr_bit & 0x01) == 0x00){
        nokia_lcd_write_string(" 0", 1);
        //nokia_lcd_render();
        //nokia_lcd_clear();
      } else {
        nokia_lcd_write_string(" 1", 1);
        //nokia_lcd_render();
        //nokia_lcd_clear();
      }
    }*/
    if(lose == 0x01 && score < 100 && on == 0x01){
      PORTD = score;
      nokia_lcd_set_cursor(0, 40);
      nokia_lcd_write_string("You lose!", 1);
      //nokia_lcd_render();
      //nokia_lcd_clear();
    } else if (lose == 0x00 && score >= 100 && on == 0x01){
      PORTD = score;
      nokia_lcd_set_cursor(0, 40);
      nokia_lcd_write_string("You win!", 1);
      //nokia_lcd_render();
      //nokia_lcd_clear();
    } else if(on == 0x00){
      PORTD = 0x00;
    }
    break;

    default:
    break;
  }
  nokia_lcd_render();
  nokia_lcd_clear();
	return state;
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00; //LEDs
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;  //Ledds

	static task task1, task2, task3, task4, task5;
	task *tasks[] = {&task1, &task2, &task3, &task4, &task5};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	const char start = -1;

  task1.state = start;
  task1.period = 150;
  task1.elapsedTime = task1.period;
  task1.TickFct = &MonitorSMTick;

  task2.state = start;
	task2.period = 150;
	task2.elapsedTime = task2.period;
	task2.TickFct = &JoystickSMTick;

  task3.state = start;
	task3.period = 300;
	task3.elapsedTime = task3.period;
	task3.TickFct = &MatrixSMTick;



	      task4.state = start;
        task4.period = 150;
        task4.elapsedTime = task4.period;
        task4.TickFct = &GameSMTick;

        task5.state = start;
        task5.period = 150;
        task5.elapsedTime = task5.period;
        task5.TickFct = &OutputSMTick;


	unsigned long GCD = tasks[0]->period;
	for(unsigned char i = 1; i <numTasks; ++i){
		GCD = findGCD(GCD,tasks[i]->period);
	}
	TimerSet(GCD);
	TimerOn();
	unsigned short i;

	ADC_init();

  	nokia_lcd_init();
  nokia_lcd_clear();
/*  nokia_lcd_write_string("Nice!", 3);
  nokia_lcd_render();

  for (;;) {
      _delay_ms(1000);
  }*/


    /* Insert your solution below */
    while (1) {
      //nokia_lcd_write_string("Press button", 2);
      //nokia_lcd_render();
      //nokia_lcd_clear();
	    for(i = 0; i < numTasks; ++i){
		    if(tasks[i]->elapsedTime == tasks[i]->period){
			    tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
			    tasks[i]->elapsedTime = 0;
		    }
		    tasks[i]->elapsedTime += GCD;
	    }
		    while(!TimerFlag);
		    TimerFlag = 0;

    }
    return 1;
}
