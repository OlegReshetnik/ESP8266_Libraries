/*
Connection
+----------+
| FRONT    |
+----------+
 |  |  |  |
 1  2  3  4

ESP8266  DHT12
vcc      1
4-SDA    2
gnd      3
5-SCL    4
*/

#pragma once
#include <Wire.h>

#define DHT12_DEFAULT_ADDRESS	0x5c // default I2C adress
#define DHT12_UPDATE_INTERVAL	1000 // 1 second

class Dht12 {
public:
	Dht12( uint8_t address = DHT12_DEFAULT_ADDRESS );
	float getTemperature();
	float getHumidity();

private:
	void read();
	uint8_t Dht12Address;
	uint8_t data[5];
	uint32_t oldRead;
};
