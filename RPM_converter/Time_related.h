#ifndef TIME_RELATED_H_
#define TIME_RELATED_H_
#include <stdint.h>

#define MILLIS_RANGE 1000
#define SECOND_RANGE 60
#define MINUTE_RANGE 60
#define HOUR_RANGE 24



// Create a class holding the date of the day (all members initialized to 0 when instantiated)
class date {
	public:
	date();
	date(uint16_t ms, uint8_t second, uint8_t minute , uint8_t hour);
	uint16_t ms;
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	void reset();
	date substract(date &other );
	void add_date(date new_time);
	void add_date(date new_time) volatile ;
	
	void inc_ms();
	void inc_ms() volatile;				// same for volatile expressions
	
	date divide(uint8_t division_factor);
	uint32_t convert_to_ms();
	
	// Overloading operator with volatile is not permitted
	// This will cause the compilation to fail
	void operator=(const date& my_date);	// regular assignment operator
	date operator-(const date& my_date);
	date operator+(const date& my_date);
	date operator/(const uint8_t division_factor);
	date cast_from_volatile(const volatile date& other);
	void operator=(volatile const date& other) ;			// Reads from a volatile and converts it back to regular date (non-volatile)
	void operator=(const date& other) volatile;	// Converts a standard date (non-volatile) to volatile type
	void operator=(volatile const date& other) volatile;	// Assigns one volatile date with another one (volatile as well)
};

date find_middle(date& time1, date& time2);



#endif /* TIME_RELATED_H_ */