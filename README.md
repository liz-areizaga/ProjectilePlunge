## ProjectilePlunge

[Video Demo:](https://www.youtube.com/watch?v=LOXY49CsT1s)

The projectile plunge game consists of obstacles moving up toward your character which is displayed on
8 LED lights which align with the 8 columns of the LED matrix. Your goal is to dodge 10 obstacles. If you
wish to reset the game at any time you can press the button provided. When the game is over, meaning
the player loses or wins, their score will be displayed on the screen.

• Button:
  - Connected to A2
  - Start and reset button
  - The button can be pressed at any time. When the game is at the start screen,
pressing the button starts the game and the user can immediately start playing. When the game is on, the button resets the game to the start screen. When the button is pressed after player loses or wins, it resets the game to the start
screen.
• Joystick:
  - Connected to A[0:1]
  - Used to move the character
  - When the joystick is moved right, the character moves right every 150ms. If the user moves the character right once and proceeds to recenter it, the character will move one LED to the right of its original position. If the user holds the joystick to the right, the character will move until it reaches the right-most position. The joystick acts the same way when moved to the left respectively. When the joystick is moved up or down, the game does nothing. Joystick only
takes left or right input since the character can only move left or right.

• LCD:
  - Connected to B[1:5]
  - Used to display instructions for button, score, and message to let the user know if they won or lost.
  - When in the start screen the LCD screen reads “Welcome! Press button to
start.” When the game is in play it reads “Press button to reset.” When the
winner wins or loses, the LCD screen reads “Press the button to reset. \n Score :
<score> \n You <win/lose>!”.

• LED lights:
  - Connected to D[0:7]
  - Used to display the character.
  - Connected to the joystick, so that it reads user input and moves the character left or right accordingly.

• 8x8 Matrix:
  - Connected to 2 shift registers which are daisy chained and connected to C[0:2]
  - Used to display the obstacles.
  - The matrix outputs six different patterns of obstacles which the user must dodge. The matrix continues to loop through the same six patterns.

# User Guide:
Rules:
1) Press the button to begin playing. Press the button at any time while you are playing to
reset to the start screen.
2) Move the joystick left or right to move your character (up and down motion is ignored).
3) Dodge the obstacles that are moving toward you.
4) If an obstacles reaches you, you lose.
5) If you reach 10 points you win! Note: A 10 is the maximum score and it is displayed as a ‘*’
on the LCD screen (this is because if you win, you’re a star!).
6) You accumulate 1 points every time you successfully dodge an obstacle.
7) Your score will be displayed on the screen after the game if over, then you can press the
button to return the game to the start screen.
Controls:
Your control is the joystick which will read your input when you move it left or right.
Special Considerations:
The joystick does not read up and down input since the character can only move left and right.

# Technologies:

- AVR Studio 6
-  Atmega 1284 Microcontroller
-  Nokia 5110 LCD screen
-  SN 74HC595N shift registers
-  2-axis Joystick
-  Button
-  Green LED lights
-  8x8 LED RGB matrix

# Source File Links:

-  main.c - Contains the state machines and implements the game.
-  timer.h – Controls the timer which controls the ticks of the state machines. Code
obtained from instructor.
-  nokia5110.h – contains the declaration of functions used to control the LCD screen.
Code obtained from: GitHub - LittleBuster/avr-nokia5110: Lightweight and easy to use
library for PCD8544 lcd controller, tested on ATmega328P and Nokia 5110 LCD
-  nokia5110.c – contains the definitions of functions used to control the LCD screen. Code
obtained from: GitHub - LittleBuster/avr-nokia5110: Lightweight and easy to use library
for PCD8544 lcd controller, tested on ATmega328P and Nokia 5110 LCD
-  nokia5110_chars.h – contains a two-dimensional array used to write characters to the
LCD screen. Code obtained from: GitHub - LittleBuster/avr-nokia5110: Lightweight and
easy to use library for PCD8544 lcd controller, tested on ATmega328P and Nokia 5110
LCD

# Technologies Learned:
While completing my custom lab project, I was able to explore how to use an Atmega 1284
microcontroller with an AVR simulator. As I progressed through the project, I was able to learn how to
use shift registers to optimize port usage. I successfully implemented reading ADC input to control the
movement of a joystick.
