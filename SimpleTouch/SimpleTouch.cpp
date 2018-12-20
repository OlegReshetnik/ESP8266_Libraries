#include "SimpleTouch.h"

SimpleTouch::SimpleTouch( uint8_t touchPin )
{
	__pin = touchPin;
	calibrateTouch();
}

uint32_t SimpleTouch::__getOneTouch(void) {
	uint16_t tck = 0;
	uint32_t mask = TOUCH_PIN_MASK( __pin );

	TOUCH_PIN_ZERO(__pin);
	TOUCH_PIN_INPUT(__pin);

	while( IS_TOUCH_PIN_ZERO(mask) ) { 
		if( ++tck > MAX_COUNTS ) return MAX_COUNTS;
	}

	return tck;
}

uint32_t SimpleTouch::getTouch(void) {
	uint32_t sum = 0;
	for( int i=0; i < TOUCH_REPEATS; i++ ) { sum += __getOneTouch(); }
	return sum/TOUCH_REPEATS;
}

void SimpleTouch::calibrateTouch(void) {
	uint32_t sum = 0;
	for( int i=0; i < TOUCH_REPEATS; i++ ) { sum += getTouch(); }
	__calibrate = sum/TOUCH_REPEATS;
}

bool SimpleTouch::isTouch(void) {
	uint32_t cnt = getTouch();
//	if( cnt < (__calibrate * 2 / 3) || cnt > (__calibrate * 4 / 3) ) return true;
	if( cnt < (__calibrate / 2) ) return true;
	return false;
}
