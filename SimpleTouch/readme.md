Библиотека простейшей сенсорной кнопки.

Использование:
1. Можно использовать только пины, которые не поддтянуты к 0 или +
2. Между пином и +3.3 в подсоединить ресистор 1 мегаом
3. От пина провод подключается к площадке площадью около 2 см2

#include <SimpleTouch.h>

#define TOUCH_PIN 13 // wemos D7

SimpleTouch myTouch( TOUCH_PIN );

void setup() {
	Serial.begin(115200);
}

void loop() {
	if( myTouch.isTouch() ) { Serial.println( "Touch!" ); }
	delay(500);
}
