/*	Author: Lizbeth Areizaga
 *  Partner(s) Name:
 *	Lab Section:021
 *	Assignment: Project  Demo #2
 *	Exercise Description: Joystick left and right movements needed for game complete in SM form.
 *                        demo with a line of leds. Starts at b0, left is in the direction of msb right is in the direction of lsb
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo 1 URL: https://youtu.be/6xqISCjyJo4
 *	Demo 2 URL: https://youtu.be/O8Q8D7RK79c
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

void sendToSR(unsigned char data){ //for LED rows c0 to c2
  PORTC &= 0xFD; //RCLKL = 0 set C1 to 0

  for(unsigned char i = 0; i < 8; ++i){
    PORTC = (PORTC & 0xFE) | GetBit(data, (7-i)); //SER gets value of data at certain position
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
unsigned char row = 0x00;
unsigned char on = 0;

enum Monitor_States {Monitor_Start, Monitor_off, Monitor_onp, Monitor_onr, Monitor_resetp};
int MonitorSMTick(int state){
	unsigned char button = ~PINA & 0x04;
	switch(state){
		case Monitor_Start:
			state = Monitor_off;
			on = 0;
			sendToSR(0xFF);
                        sendToSR(0x00);
			break;

		case Monitor_off:
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
			}
			break;

		case Monitor_onr:
			if(!button){
				state = Monitor_onr;
			} else {
				state = Monitor_resetp;
			}
			break;

		case Monitor_resetp:
			if(button){
				state = Monitor_resetp;
			} else {
				state = Monitor_off;
				sendToSR(0xFF);
                        	sendToSR(0x00);
			}
			break;

		default:
			state = Monitor_Start;
			break;

	}

	switch(state){
		case Monitor_off:
			on = 0x00;
			break;

		case Monitor_onr:
			on = 0x01;
			break;

		default:
			break;
	}
	return state;
}

enum Joystick_States {Joystick_start, Joystick_wait, Joystick_right, Joystick_left};
int JoystickSMTick(int state){
	  unsigned short adc_val = 0;
		adc_val = ADC;
		ADMUX = 1; // for port a1, connected to left and right
		unsigned short x_val = adc_val;

	switch (state){
			case Joystick_start:
			led = 0x01;
			state = Joystick_wait;
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
			break;

			case Joystick_left:
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
			break;

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
			break;

			default:
			state = Joystick_start;
			break;
	}

return state;
}
unsigned char j = 0;

enum Matrix_States {Matrix_Start, Matrix_scroll};
int MatrixSMTick(int state){
  switch(state){
    case Matrix_Start:
    state = Matrix_scroll;
    row = 0x80;
    j = 0;
    break;

    case Matrix_scroll:
    state = Matrix_scroll;
    break;

    default:
    state = Matrix_Start;
    break;
  }

  switch (state){
    case Matrix_scroll:
    if ((row & 0x01) == 0x01){
      if(j < byteSize -1 ){
                ++j;
       } else {
	       j = 0;
       }
    }
    if(row > 1){
     row = (row >> 1);
    } else {
	    row = 0x80;
    }
    break;

    default:
    break;
  }

  return state;
}

enum Output_States {Output_Start, Output_display};
int OutputSMTick (int state){
	switch (state){
		case Output_Start:
		PORTD = 0x00;
		state = Output_display;
		break;

		case Output_display:
		if(on == 0x01){
			PORTD = led;
			sendToSR(~row);
			sendToSR(~byte[j]);
		} else {
			PORTD = 0x00;
		}
		state = Output_display;
		break;

		default:
		state = Output_Start;
		break;
	}
	return state;
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	//DDRB = 0xFF; PORTB = 0x00; //LEDs
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;  //Ledds

	static task task1, task2, task3, task4;
	task *tasks[] = {&task1, &task2, &task3, &task4};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	const char start = -1;

	task1.state = start;
	task1.period = 150;
	task1.elapsedTime = task1.period;
	task1.TickFct = &JoystickSMTick;

	task2.state = start;
	task2.period = 150;
	task2.elapsedTime = task2.period;
	task2.TickFct = &OutputSMTick;

  task3.state = start;
	task3.period = 300;
	task3.elapsedTime = task3.period;
	task3.TickFct = &MatrixSMTick;

	task4.state = start;
        task4.period = 100;
        task4.elapsedTime = task4.period;
        task4.TickFct = &MonitorSMTick;

	unsigned long GCD = tasks[0]->period;
	for(unsigned char i = 1; i <numTasks; ++i){
		GCD = findGCD(GCD,tasks[i]->period);
	}
	TimerSet(GCD);
	TimerOn();
	unsigned short i;

	ADC_init();

  /*nokia_lcd_init();
  nokia_lcd_clear();
  nokia_lcd_write_string("Nice!", 3);
  nokia_lcd_render();

  for (;;) {
      _delay_ms(1000);
  }*/


    /* Insert your solution below */
    while (1) {
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
