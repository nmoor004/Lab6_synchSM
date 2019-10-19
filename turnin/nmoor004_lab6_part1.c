/*	Author: nmoor004
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>


volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C Programmer should clear to 0.

unsigned long _avr_timer_M = 1;	       // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

enum LED_States {LED_Init, LED_B0, LED_B1, LED_B2} LED_State;

void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B;     // bit3 = 0: CTC mode (clear timer on compare)
		          // bit2bit1bit0 = 011: pre-scaler /64
			 // 00001011: 0x0B
			// SO, 8 MHz clock or 8,000,000 / 64 = 125,000 ticks/s
		       // Thus, TCNT1 register will count at 125,000 tick/s

	// AVR output compare register OCR1A.
	OCR1A = 125;    // Timer interrupt will be generated when TCNT1==OCR1A
		       // We want a 1 ms tick. 0.001s * 125,000 ticks/s = 125
		      // So when TCNT1 register equals 125,
		     // 1 ms has passed. Thus, we compare to 125.

	// AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	// Initialize avr counter
	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds

	// Enable globla interrupts
	SREG |= 0x80; // 0x80: 1000000
}

void TimerOff() {
	TCCR1B = 0x00; /// bit3bit1bit0 = 000: timer off
}


void TimerISR() {
	TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP

	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); 	      // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}

}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void Tick_LED() {
	switch(LED_State) { //Mealy Transitions
		case LED_Init: 
			PORTB = (PORTB & 0x00);
			LED_State = LED_B0;
			break;
		case LED_B0:
			PORTB = (PORTB & 0x00) | 0x01;
			LED_State = LED_B1;
			break;
		case LED_B1:
			PORTB = (PORTB & 0x00) | 0x02;
			LED_State = LED_B2;
			break;
		case LED_B2:
			PORTB = (PORTB & 0x00) | 0x04;
			LED_State = LED_B0;
			break;

	}

	switch(LED_State) { //Mealy Actions
		case LED_Init:
			break;
		case LED_B0:
			break;
		case LED_B1:
			break;
		case LED_B2:
			break;
	}

}

int main(void) {    /* Insert DDR and PORT initializations */
	DDRB = 0xFF; PORTB = 0x00; // output
	TimerSet(1000);
	TimerOn();

    /* Insert your solution below */
    while (1) {
	Tick_LED();
	while (!TimerFlag); // Wait 1 sec
	TimerFlag = 0;
 	 // Note: For the above a better style would use a synchSM with TickSM()
	// This example just illusrates the use of ISR and flag.
    }
return 1;
}
