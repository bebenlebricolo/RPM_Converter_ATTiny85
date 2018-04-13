/*
 * Time_related.cpp
 *
 */ 
#include "Time_related.h"

// First initialize everything to 0
date::date():ms(0),second(0),minute(0),hour(0){}
	
date::date(uint16_t n_ms, uint8_t n_second, uint8_t n_minute , uint8_t n_hour) : ms(n_ms),second(n_second),minute(n_minute),hour(n_hour){}
void date::reset(){
	ms=0;
	second=0;
	minute=0;
	hour = 0;
}
date date::substract(date &other ){
	date result;
	if(ms < other.ms) result.ms = other.ms - ms;
	else result.ms = ms - other.ms;
	
	if(second < other.second) result.second = other.second - second;
	else result.second = second - other.second;
	
	if(minute < other.minute) result.minute = other.minute - minute;
	else result.minute = minute - other.minute;
	
	if(hour < other.hour) result.hour = other.hour - hour;
	else result.hour = hour - other.hour;
	
	return result;
}

// Adding two dates
void date::add_date(date new_time){
	ms += new_time.ms;
	if(ms >= 1000) {
		ms %= 1000;
		second++;}
	else return;
	second += new_time.second;
	if(second >= 60) {
		second %= 60;
		second++;
	}
	else return;
	minute += new_time.minute;
	if(minute >= 60) {
		minute %= 60;
		minute++;
	}
	else return;
	hour += new_time.hour;
	if(hour >= 24){
		hour %= 24;	// new day
	}
}

void date::add_date(date new_time) volatile {
	ms += new_time.ms;
	if(ms >= 1000) {
		ms %= 1000;
	second++;}
	else return;
	second += new_time.second;
	if(second >= 60) {
		second %= 60;
		second++;
	}
	else return;
	minute += new_time.minute;
	if(minute >= 60) {
		minute %= 60;
		minute++;
	}
	else return;
	hour += new_time.hour;
	if(hour >= 24){
		hour %= 24;	// new day
	}
}

void date::inc_ms() 
{
	add_date(date(1,0,0,0));
}

void date::inc_ms() volatile
{
	add_date(date(1,0,0,0));
}

// Finds the middle of a time range (useful to center pulses)
// TODO : verify if this version of division is correct (works with 2 but only 2)
// -> Done! Works like a charm =)
date date::divide(uint8_t division_factor) {
	date result;
	result.ms = ms / division_factor + (second % division_factor)*MILLIS_RANGE / division_factor;
	result.second = second / division_factor + (minute % division_factor) * SECOND_RANGE / division_factor;
	result.minute = minute / division_factor + (hour % division_factor) * MINUTE_RANGE / division_factor;
	result.hour = hour / division_factor;
	return result;
}

// Cast to milliseconds
uint32_t date::convert_to_ms()
{
	uint32_t result = 0;
	result = ms + 1000*(second + 60*(minute + 24*(hour)));
}

//////////////////////////////////////////////////////////////////////////
// operators on volatile objects
// Assignation of two dates (aka copy operator)
void date::operator=(const date& my_date){
	ms = my_date.ms;
	second = my_date.second;
	minute = my_date.minute;
	hour = my_date.hour;
}

// substraction of two dates
date date::operator-(const date& my_date){
	int16_t res_ms;
	int8_t res_sec;
	int8_t res_min;
	int8_t res_hr;
	
	res_ms = ms - my_date.ms;
	res_sec = second - my_date.second;
	res_min = minute - my_date.minute;
	res_hr = hour - my_date.hour;
	if(res_ms < 0) {
		res_ms += 1000;
		res_sec--;
	}
	if(res_sec < 0) {
		res_sec += 60;
		res_min--;
	}
	if(res_min < 0) {
		res_min += 60;
		res_hr--;
	}
	if(res_hr < 0) res_hr += 24;
	date result(res_ms,res_sec,res_min,res_hr);
	return result;
}

// Addition of two dates
date date::operator+(const date& my_date){
	date result;
	result.ms += ms + my_date.ms;
	// handling case where result.ms greater than 1000 ms (eg: 1423 -> 1sec + 423 ms)
	if(result.ms > 1000){
	result.ms %= 1000;
	result.second++;	// increment second's counter
	}
	result.second += second + my_date.second;
	// Repeat the same process for second, minute and hour
	if(result.second > 60){		
		result.second %= 60;
		result.minute++;
	}
	result.minute += minute + my_date.minute;
	if(result.minute > 60){
		result.minute %= 60;
		result.hour++;
	}
	result.hour += hour + my_date.hour;
	result.hour %= 24;
	
	return result;
}

date date::cast_from_volatile(const volatile date& other){
	date result(other.ms,other.second,other.minute,other.hour);
	return result;
}

date date::operator /(const uint8_t division_factor){
	date result;
	result = divide(division_factor);
	return result;
}


void date::operator=(volatile const date& other)  {
	//date result(other.ms, other.second,other.minute,other.hour);
	ms = other.ms;
	second = other.second;
	minute = other.minute;
	hour = other.hour;
}

void date::operator=(const date& other) volatile {
	//date result(other.ms, other.second,other.minute,other.hour);
	ms = other.ms;
	second = other.second;
	minute = other.minute;
	hour = other.hour;
}

void date::operator=(volatile const date& other) volatile {
	//date result(other.ms, other.second,other.minute,other.hour);
	ms = other.ms;
	second = other.second;
	minute = other.minute;
	hour = other.hour;
}


date find_middle(date& time1, date& time2){
	date result;
	result = time1 + time2;
	result = result.divide(2);
	return result;
}