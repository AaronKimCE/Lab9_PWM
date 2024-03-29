/*	Author: akim106
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 9  Exercise 1
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

enum States{Wait, Held} State;
unsigned char input;

void Tick() {
    input = ~PINA & 0x07;
    switch(State) { // State Transitions
      case Wait:
        if (input == 0x01 || input == 0x02 || input == 0x04) {
          State = Held;
        } else {
          State = Wait;
        }
        break;
      case Held:
        if (input == 0x01 || input == 0x02 || input == 0x04) {
          State = Held;
        } else {
          State = Wait;
        }
        break;
    }
    switch(State) { // State Actions
      case Wait:
        set_PWM(0);
        break;
      case Held:
        if (input == 0x01) {
          set_PWM(261.63); //C Note
        } else if (input == 0x02) {
          set_PWM(293.66); //D Note
        } else {
          set_PWM(329.63); //E Note
        }
        break;
    }
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF; // Input
    DDRB = 0xFF; PORTB = 0x00; // Output
    PWM_on();
    State = Wait;

    while (1) {
      Tick();
    }
    return 1;
}
