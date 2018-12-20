/*
Использование:
1. Можно использовать только пины, которые не поддтянуты к 0 или +
2. Между пином и +3.3 в подсоединить ресистор 1 мегаом
3. От пина провод подключается к площадке площадью около 2 см2
*/

#pragma once
#include <Arduino.h>

#define TOUCH_REPEATS	20
#define MAX_COUNTS		5000

#define TOUCH_PIN_MASK( PIN )		(1 << PIN)
#define TOUCH_PIN_ZERO( PIN )		do { pinMode( PIN, OUTPUT ); digitalWrite( PIN, 0 ); } while(0)
#define TOUCH_PIN_INPUT( PIN )		pinMode( PIN, INPUT )
#define IS_TOUCH_PIN_ZERO( MASK )	( !( GPI & MASK ) )

class SimpleTouch
{
	public:
		SimpleTouch( uint8_t touchPin );
		uint32_t getTouch(void);
		void calibrateTouch(void);
		bool isTouch(void);

	private:
		uint32_t __getOneTouch(void);
		uint8_t __pin;
		uint32_t __calibrate;
};
