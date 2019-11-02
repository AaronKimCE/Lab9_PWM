/*	Author: akim106
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 9  Exercise 2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void set_PWM(double frequency) {
    static double current_frequency;
    if (frequency != current_frequency) {
      if (!frequency) {TCCR3B &= 0x08;}
      else {TCCR3B |= 0x03;}
      if (frequency < 0.954) {OCR3A = 0xFFFF;}
      else if (frequency > 31250) {OCR3A = 0x0000;}
      else {OCR3A = (short) (8000000 / (128 * frequency)) - 1;}

      TCNT3 = 0;
      current_frequency = frequency;
    }
}

void PWM_on () {
    TCCR3A = (1 << COM3A0);
    TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
    set_PWM(0);
}

void PWM_off() {
    TCCR3A = 0x00;
    TCCR3B = 0x00;
}

enum States{Off, On, TurnOff, TurnOn, PressUp, PressDown, Inc, Dec} State;
unsigned char input;
unsigned char position;
const double NoteArray[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};

void Tick() {
    input = ~PINA & 0x07; // On/Off - 0x01, Up - 0x02, Down - 0x04
    switch(State) { // State Transitions
      case Off:
        if (input == 0x01) {
          State = TurnOn;
        } else {
          State = Off;
        }
        break;
      case TurnOn:
        if (input == 0x00) {
          State = On;
          position = 0;
        } else if (input != 0x01) {
          State = Off;
        } else {
          State = TurnOn;
        }
        break;
      case On:
        if (input == 0x02) {
          State = PressUp;
        } else if (input == 0x04) {
          State = PressDown;
        } else if (input == 0x01) {
          State = TurnOff;
        } else {
          State = On;
        }
        break;
      case TurnOff:
        if (input == 0x00) {
          State = Off;
        } else if (input != 0x01) {
          State = On;
        } else {
          State = TurnOff;
        }
        break;
      case PressUp:
        if (input == 0x00) {
          State = Inc;
        } else if (input == 0x02) {
          State = PressUp;
        } else {
          State = On;
        }
        break;
      case PressDown:
        if (input == 0x00) {
          State = Dec;
        } else if (input == 0x04) {
          State = PressDown;
        } else {
          State = On;
        }
      case Inc:
        State = On;
        break;
      case Dec:
        State = On;
        break;
    }
    switch(State) { // State Actions
      case Off:
        set_PWM(0);
        break;
      case TurnOn:
        break;
      case On:
        set_PWM(NoteArray[position]);
        break;
      case TurnOff:
        break;
      case PressUp:
        break;
      case PressDown:
        break;
      case Inc:
        if (position < 7) {
          position++;
        }
        break;
      case Dec:
        if (position > 0) {
          position--;
        }
        break;
    }
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF; // Input
    DDRB = 0xFF; PORTB = 0x00; // Output
    PWM_on();
    State = Off;

    while (1) {
      Tick();
    }
    return 1;
}
