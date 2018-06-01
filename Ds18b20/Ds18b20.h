#ifndef Ds18b20_h
#define Ds18b20_h

#include <Arduino.h>
#define ONEWIRE_SEARCH 0 // disable search function
#include <OneWire.h>

#define DS18B20_UPDATE_INTERVAL	1000 // 1 second

class Ds18b20 {
	public:
		Ds18b20( uint8_t pin, bool isVcc = true ); // true, if +vcc, else false, if two wire connect
		~Ds18b20();
		float getTemperature(); // read temperature in Celsius
	protected:
		void _read();
	private:
		OneWire * ds;
		float _temp;
		uint32_t _oldRead;
		bool _parazitPower;
};

#endif
