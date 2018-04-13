/*
 * Pulse_compute_tools.cpp
 *
 * Created: 11/01/2018 10:19:59
 *  Author: Utilisateur
 */ 

#include "Pulse_compute_tools.h"
#include "Time_related.h"
#include <stdint.h>

volatile date cur_time;
volatile date detection_time_buffer;	// Holds current time of detection (either rising edge or falling edge)
volatile uint8_t rising_edge_detected = 0;	// Binary detection witnesses (1 = detected, 0 = nothing detected)
volatile uint8_t falling_edge_detected = 0;

date rising_time;		// Rising and falling edges times
date falling_time;
date date_period;
date old_time ;        // Variables holding the old cur_time.ms value
date new_time ;


void compute_period(void)
{
	if(rising_edge_detected || falling_edge_detected)
	{
		if(rising_edge_detected)
		{
			rising_time = detection_time_buffer;
			rising_edge_detected = 0;
			falling_edge_detected = 0;
		}
		else if(falling_edge_detected)
		{
			falling_time = detection_time_buffer;		// Extract falling_time from time buffer
			new_time = find_middle(rising_time,falling_time) ;
			rising_time.reset();
			falling_time.reset();
			
			date_period = new_time - old_time;
			// Computes the currently calculated period
			cur_period = date_period.ms + 1000 * date_period.second;
			// and stores back the timing of this current pulse (which would be the "old" one at next triggering)
			old_time = new_time;
			// Implementation of a regular filter.
			if(cur_period > MIN_period && cur_period < MAX_period)
			{
				push_period(cur_period);
			}
			else
			{
				if(cur_period < MIN_period) cur_period = MIN_period;
				if(cur_period > MAX_period) cur_period = MAX_period;
				push_period(cur_period);
			}
		}
	}
}