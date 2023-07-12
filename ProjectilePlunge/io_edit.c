#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "io.h"

#define SET_BIT(p,i) ((p) |= (1 << (i)))
#define CLR_BIT(p,i) ((p) &= ~(1 << (i)))
#define GET_BIT(p,i) ((p) & (1 << (i)))

/*-------------------------------------------------------------------------*/

/*#define DATA_BUS PORTC		// port connected to pins 7-14 of LCD display
#define CONTROL_BUS PORTD	// port connected to pins 4 and 6 of LCD disp.
#define RS 6			// pin number of uC connected to pin 4 of LCD disp.
#define E 7			// pin number of uC connected to pin 6 of LCD disp.*/ //from lab 7

//redefining for custom lab proj using nokia 5110 LCD screen
#define _BUS PORTD

#define SCE 0
#define RST 1
#define DC 2
#define DN 3 //data
#define SCLK 4

/*-------------------------------------------------------------------------*/

void LCD_ClearScreen(void) {
   LCD_WriteCommand(0x01);
}

void LCD_init(void) {

    //wait for 100 ms.
	/*delay_ms(100);
	LCD_WriteCommand(0x38);
	LCD_WriteCommand(0x06);
	LCD_WriteCommand(0x0f);
	LCD_WriteCommand(0x01);
	delay_ms(10);*/

  delay_ms(100);
  LCD_WriteCommand(0x21);
  LCD_WriteCommand(0xBF);
  LCD_WriteCommand(0x04);
  LCD_WriteCommand(0x14);
  LCD_WriteCommand(0x0C);
  LCD_WriteCommand(0x20);
  LCD_WriteCommand(0x0C);
  delay_ms(10);

}

void LCD_WriteCommand (unsigned char Command) { //first it is clearing reset andd then setting it high
   CLR_BIT(_BUS,RST); //this clears the bit at which the reset button is so you just have to clear the reset
   //DATA_BUS = Command;
   _BUS = _BUS & (Command << 3);
   SET_BIT(_BUS,DC);
   asm("nop");
   CLR_BIT(_BUS,DC);
   delay_ms(2); // ClearScreen requires 1.52ms to execute
}

void LCD_WriteData(unsigned char Data) {
   SET_BIT(_BUS,RST);
   //DATA_BUS = Data;
   _BUS = _BUS & (Data << 3);
   SET_BIT(_BUS,DC);
   asm("nop");
   CLR_BIT(_BUS,DC);
   delay_ms(1);
}
//test
void LCD_WriteData(unsigned char Data) {
   SET_BIT(_BUS,RST);
   //DATA_BUS = Data;
   _BUS = _BUS | Data;
   SET_BIT(_BUS,DC);
   asm("nop");
   CLR_BIT(_BUS,DC);
   delay_ms(1);
}

void LCD_DisplayString( unsigned char column, const unsigned char* string) {
   LCD_ClearScreen();
   unsigned char c = column;
   while(*string) {
      LCD_Cursor(c++);
      LCD_WriteData(*string++);
   }
}

void LCD_Cursor(unsigned char column) {
   if ( column < 17 ) { // 16x1 LCD: column < 9
						// 16x2 LCD: column < 17
      LCD_WriteCommand(0x80 + column - 1);
   } else {
      LCD_WriteCommand(0xB8 + column - 9);	// 16x1 LCD: column - 1
											// 16x2 LCD: column - 9
   }
}
void LCD_Cursor(unsigned char column, unsigned char row) {

      LCD_WriteCommand(0x80 | column);
      LCD_WriteCommand(0x40 | row);
}

void delay_ms(int miliSec) //for 8 Mhz crystal

{
    int i,j;
    for(i=0;i<miliSec;i++)
    for(j=0;j<775;j++)
  {
   asm("nop");
  }
}
