/*	Author: Lizbeth Areizaga
 *  Partner(s) Name:
 *	Lab Section:021
 *	Assignment: Project  Demo #1
 *	Exercise Description: Joystick left and right movements needed for game complete in SM form
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo URL: https://youtu.be/FCa3KqdtFig
 */
#include <avr/io.h>
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

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

//global variable to get key
unsigned char led = 0;

enum Joystick_States {Joystick_start, Joystick_wait, Joystick_right, Joystick_left};
int JoystickSMTick(int state){
	  unsigned short adc_val = 0;
		adc_val = ADC;
		ADMUX = 1; // for port a1, connected to left and right
		unsigned short x_val = adc_val;

	switch (state){
			case Joystick_start:
			led = 0x00;
			state = Joystick_wait;
			break;

			case Joystick_wait:
			if(x_val > 800 ){
				state = Joystick_right;
				led = 0x02;
			} else if (x_val < 50){
				state = Joystick_right;
				led = 0x04;
			}else if (x_val >= 50 && x_val <=800){
				state = Joystick_wait;
				led = 0x00;
			}
			break;

			case Joystick_left:
			if(x_val > 800 ){
				state = Joystick_right;
				led = 0x02;
			} else if (x_val < 50){
				state = Joystick_right;
				led = 0x04;
			}else if (x_val >= 50 && x_val <=800){
				state = Joystick_wait;
				led = 0x00;
			}
			break;

			case Joystick_right:
			if(x_val > 800 ){
				state = Joystick_right;
				led = 0x02;
			} else if (x_val < 50){
				state = Joystick_right;
				led = 0x04;
			}else if (x_val >= 50 && x_val <=800){
				state = Joystick_wait;
				led = 0x00;
			}
			break;

			default:
			state = Joystick_start;
			break;
	}

return state;
}

enum Output_States {Output_Start, Output_display};
int OutputSMTick (int state){
	switch (state){
		case Output_Start:
		PORTB = 0x00;
		state = Output_display;
		break;

		case Output_display:
		PORTB = led;
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
	DDRB = 0xFF; PORTB = 0x00; //output LEDs to check if code works accordingly


	static task task1, task2;
	task *tasks[] = {&task1, &task2};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	const char start = -1;

	task1.state = start;
	task1.period = 20;
	task1.elapsedTime = task1.period;
	task1.TickFct = &JoystickSMTick;

	task2.state = start;
	task2.period = 20;
	task2.elapsedTime = task2.period;
	task2.TickFct = &OutputSMTick;

	unsigned long GCD = tasks[0]->period;
	for(unsigned char i = 1; i <numTasks; ++i){
		GCD = findGCD(GCD,tasks[i]->period);
	}
	TimerSet(GCD);
	TimerOn();
	unsigned short i;

	ADC_init();


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
