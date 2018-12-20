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
