/*      Author: Lizbeth Areizaga
 *  Partner(s) Name:
 *      Lab Section:021
 *      Assignment: Project Demo 1
 *      Exercise Description: Full functionality of the right and left joystick movements needed to play the game Demonstrated by four leds organized in a diamond shape.
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 *
 *      Demo URL: https://youtu.be/WObgFf6TEK4
 */
 #include <avr/io.h>
	  #ifdef _SIMULATE_
    #include "simAVRHeader.h"
    #endif
          //the following three lines were borrowed from given code from lab 8 of CS 120B class
          void ADC_init(){
                  ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
          }

          int main(void) {
              /* Insert DDR and PORT initializations */
                  DDRA = 0x00; PORTA = 0xFF; //joystick input
                  DDRB = 0xFF; PORTB = 0x00; //outputs for led, B0-up, B1-right, B2-left, B3-down
                unsigned short x_val = 0;
              /* Insert your solution below */
                  ADC_init();
                  unsigned short adc_val = 0;
                  while (1) {
                    adc_val = ADC;
		                ADMUX = 1; // for port a1, connected to left and right
                    x_val = adc_val;
                    if(x_val > 800 ){
                      PORTB = 0x02;
                    } else if (x_val < 50){
                      PORTB = 0x04;
                    }else if (x_val >= 50 && x_val <=800){
                      PORTB = 0x00;
                    }


                  }
          return 1;
          }
