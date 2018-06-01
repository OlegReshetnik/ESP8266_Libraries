#include "Ds18b20.h"

Ds18b20::Ds18b20( uint8_t pin, bool isVcc ) {
	_parazitPower = !isVcc;
	ds = new OneWire( pin );
	_read();
	_temp = 20.0;
}

Ds18b20::~Ds18b20() {
	delete ds;
}

void Ds18b20::_read() {
	ds->reset();
	ds->write(0xCC); // Пропуск ROM, т.к. у нас один датчик 
	ds->write(0xBE); // Чтение регистров
	uint8_t d0 = ds->read(); // Младший байт температуры * 16
	uint8_t d1 = ds->read(); // Старший байт температуры * 16
	ds->reset();
	ds->write(0xCC);
	ds->write(0x44, _parazitPower);	// Команда измерить температуру,
									// при подключении по двум проводам используем
									// паразитное питание на время преобразования
	_temp = ( ( d1 << 8 ) | d0 ) * 0.0625;
}

float Ds18b20::getTemperature() {
	if( ( millis() - _oldRead ) > DS18B20_UPDATE_INTERVAL ) {
		_read();
		_oldRead = millis();
	}
	return _temp;
}
