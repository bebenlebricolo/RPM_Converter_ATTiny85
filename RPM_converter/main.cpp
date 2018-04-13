/*
 * RPM_Converter.cpp
 *
 * First Created: 25/10/2017 11:34:11
 * Author : Bebenlebricolo
 * 
 * Versions | Release Date  | Comments
 * *****************************
 *    0.1      25/10/2017     First Release (triggers on rising edge only) -> drives a galvanometer correctly with a clean signal (e.g. generated with a variable PWM generator)
 *    0.2      14/01/2018	  Implemented tools to find pulses center and compute period
 *    0.3      13/04/2018     Try another approach -> counting how many pulses were seen in a fixed amount of time           
 */



 /*Release notes
  ****************
  ****************
  * Version 0.1
  * 
  * Principle of working:
  * Takes a squarish signal as input and converts it into a square-wave voltage used to drive a galvanometer(through appropriate filters)
  * Input has to be a clean TTL signal(e.g. generated with a variable PWM generator for testing purposes or Hall sensor on an engine's shaft facing magnets)
  * 
  * Known issues :
  *   - Doesn't find the center of a pulse (rising edge only)
  *     -> not immune to timing variations between pulses (e.g 1st larger than 2nd). Hence output signal appears laggy
  *   - Still a lot of logic computed inside ISRs
  *
 
 */
 
#define MAX_SAVG 1        // Maximum size of the Sliding Average array
#define F_CPU 8000000UL    // Base clock frequency (internal, 8 MHz, non divided)

#define MAGNET_NB 2        // Number of magnets used for one revolution

#define MAXRPM 9000        // Maximum RPM rate. Frequencies above will not be used (saturation to 9000 RPM)
#define MINRPM 500         // All frequencies underneath this value will not be taken into account for output signal calculation

#define O_PORT PB1    // Output port (signal out)
#define I_PORT PB2    // Input port (INT signal)
#define O_DDR DDRB    // Output Data Direction Register
#define I_DDR DDRB    // Input Data Direction Register

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "Time_related.h"

 /************************************************************************/
 /* Digital to Analog related properties								 */
 /************************************************************************/
const uint16_t MIN_period = (float)60/MAXRPM*1000;  // MAXRPM = 9000 -> min_period = 6 ms
const uint16_t MAX_period = (float)60/MINRPM*1000;  // MINRPM = 500  -> max_period = 120 ms

// time length of one counting session in milliseconds ( 1/4 of a second )
const uint16_t pulse_counter_reset_time = 250; 

const uint16_t DAC_Frequency = 1000;  // 1kHz sampling rate

// PWM output signal variables
// Version 0.2 pulse_index variable not used in v 0.3
// volatile uint8_t pulse_index = 0;     // Holds the current number of pulse detected
volatile uint8_t cur_period_index = 0;    // current period index (in the array of calculated periods)
volatile uint32_t cur_period = 0;     // Holds the currently calculated period (milliseconds)
volatile uint8_t s_avg_array[MAX_SAVG];   // Array holding the periods calculated and used to compute the sliding average
volatile uint8_t avg_period = 0;      // Result of moving average
volatile uint32_t sum_period = 0;     // Sum of period values stored in the array s_avg_array
// To be implemented when voltage sensing is required
//volatile uint16_t cur_voltage = 0;      // Currently aimed output voltage
volatile uint8_t cur_duty_cycle = 0;    // Currently used duty cycle (0 - 255) -> 8 bit Timer resolution

const uint8_t MIN_DUTY = 0;         // Determines the minimum Duty cycle resolution (number of 'Ticks')
const uint8_t MAX_DUTY = 255;       // Determines the maximum Duty cycle resolution


// Declaring time-dependent variables (hold proper time of pulses)
volatile date cur_time;
volatile date detection_time_buffer;	// Holds current time of detection (either rising edge or falling edge)
volatile uint8_t rising_edge_detected = 0;	// Binary detection witnesses (1 = detected, 0 = nothing detected)
volatile uint8_t falling_edge_detected = 0;

volatile uint16_t pulse_count = 0;
volatile uint8_t pulse_counter_resets = 0;


// Initialize the s_avg_array
void init_array(void)
{
  uint8_t i = 0;
  for(i=0 ; i< MAX_SAVG ; i++ )
  {
  s_avg_array[i] = 0;
  }
}

// Pushes the new period value inside the array
void push_period(uint8_t new_period)
{
  cur_period_index++;
  cur_period_index %= MAX_SAVG;

  sum_period += new_period - s_avg_array[cur_period_index];
  avg_period = sum_period / MAX_SAVG;
  s_avg_array[cur_period_index] = new_period;
}

// Transforms "X" of the input range into " X' " mapped in the second range (output)
// Only works with positive ranges (unsigned variables)
// maximum uint16_t input values : 0 - 65535

// TODO : Having float conversion for this kind of calculation is SUPER heavy. Change for integer-only calculation.
uint16_t interpol(uint16_t X, uint16_t start_in, uint16_t end_in,uint16_t start_out, uint16_t end_out)
{
  uint16_t mapped_value = 0;
  uint16_t delta_inrange ;
  uint16_t delta_outrange ;
  if(start_in > end_in) delta_inrange = start_in - end_in;
  else delta_inrange = end_in - start_in;

  if(start_out > end_out) delta_outrange = start_out - end_out;
  else delta_outrange = end_out - start_out; 
  
  // handles a "division by 0" case
  if(delta_inrange == 0 || delta_outrange == 0)
  {
    // Returns the lowest value of output range
    mapped_value = start_out;
  }
  else
  {
    if( (int)(X - start_in) < 0)  mapped_value = uint16_t((float)(start_in - X)/delta_inrange*delta_outrange + start_out);
    else  mapped_value = uint16_t((float)(X - start_in)/delta_inrange*delta_outrange + start_out);
  }    
       return mapped_value;
}

// Triggers on rising AND falling edges
// Increments on falling edge
ISR(INT0_vect)
{
	if((PINB & ( 1 << 2 ) ) != 0) {
		pulse_count++;
	}
}

// This piece of code is used to compute the period of each pulse train.
void compute_period()
{
	cur_period = pulse_counter_reset_time / (pulse_count / MAGNET_NB) ;
}



// Initialization of the timer dedicated to hold the time
// The timer used for this purpose is the Timer / Counter 0
void init_Timekeeper(void)
{
  TCCR0A |= 1<<WGM01; // CTC OCR0A
  TCCR0B |= ( 1 << CS00) | (1 << CS01); // prescaler of 64
  TIMSK |= (1 << OCIE0A) ;        // Using OCR0A as compare match
  OCR0A = F_CPU / 64000;  // Computes the number of ticks needed to provide 1ms. 
  //OCR0A = 125;  // Computes the number of ticks needed to provide 1ms. 
  cur_time.ms = 0;      // Initializes the value of millisecond 
  TCNT0 = 0;            // Initialize the timer to the BOTTOM value
}

void init_PWM_OUT(void){
  PLLCSR = 0;       // Disables Peripheral clock source. only internal
  TCCR1 = (1 << CS12) | (1 << CS11) ; // Enabling Ck / 32 prescaler, every other bit is set to 0
  TCNT1 = 0;
  TIMSK |= (1 << OCIE1A) | (1 << OCIE1B);
  OCR1A = F_CPU/(32*DAC_Frequency);    // Maximum Tick for one period of output frequency :: 250 ticks / 1 kHz
  OCR1B = 20;
}

// Handles the time keeping routine
// This ISR is called whenever we hit the correct number of ticks (250 for 16 MHz and 125 for 8 MHz clocks)
ISR(TIMER0_COMPA_vect)
{
   cur_time.inc_ms(); // add one millisecond
   if(cur_time.ms > pulse_counter_reset_time){
	   cur_time.ms = 0;
	   pulse_counter_resets++;
   }
 }

// Timer 1 sets the output port to high state when it overflows
ISR(TIMER1_COMPA_vect){
if(cur_duty_cycle > MIN_DUTY && cur_duty_cycle < MAX_DUTY){
  PORTB |= 1 << O_PORT; // Flips the output pin to high
  TIMSK |= 1 << OCIE1B; // Enables the output compare Interruption
  OCR1B = cur_duty_cycle;
  TCNT1 = 0;
  return;
}
else if(cur_duty_cycle > MAX_DUTY){
  PORTB |= 1 << O_PORT;   // Flips the output pin to high
}
else {
  PORTB &= ~(1 << O_PORT);  // shuts down the output 
  }
  
TIMSK &= ~(1 << OCIE1B);  // Disables the output compare triggering
TCNT1 = 0;

}

// Timer 1 Output Compare A enable
ISR(TIMER1_COMPB_vect){
PORTB &= ~(1 << O_PORT); // Switches down the output pin
}

void enable_ext_int(void){
//MCUCR = (1 << ISC01) | (1 << ISC00);  // Enabling the INT0 pin to trigger on rising edge only
MCUCR = (1 << ISC00);		  // Enabling the INT0 pin to trigger on any logical change (high or low)
GIMSK |= (1 << INT0);         // Enabling INT0 to trigger the interrupt service routine on PB2 (ATtiny85)
I_DDR &= ~(1 << I_PORT);      // Enables Port B 2 as an INPUT
O_DDR |= 1 << O_PORT;         // Enables Port B 1 as an OUTPUT
PORTB &= ~(1 << O_PORT);      // Switches off output pin
}


int main(void)
{
      enable_ext_int();
      init_array();
      init_Timekeeper();
      init_PWM_OUT();
      sei();
    while (1) 
    {
		if(pulse_counter_resets != 0){
			compute_period();
			push_period(cur_period);
			pulse_counter_resets = 0 ;
		}
		
		cur_duty_cycle = interpol(avg_period, MAX_period, MIN_period, 0 , 250 );
    }
}

