#include "Dht12.h"
#include <Arduino.h>

Dht12::Dht12( uint8_t address ) {
	oldRead = 0-DHT12_UPDATE_INTERVAL;
	Dht12Address = address;
	Wire.begin();
}

void Dht12::read() {
	if( ( millis() - oldRead ) > DHT12_UPDATE_INTERVAL ) {
		Wire.beginTransmission( Dht12Address );
		Wire.write(0);
		Wire.endTransmission();
		Wire.requestFrom( Dht12Address, (uint8_t)5 );
		uint8_t i = 0;
		while( Wire.available() && i<5 ) data[i++] = Wire.read();
		oldRead = millis();
	}
}

float Dht12::getTemperature() {
	read();
	return data[2] + (float)data[3] / 10;
}

float Dht12::getHumidity() {
	read();
	return data[0] + (float)data[1] / 10;
}
