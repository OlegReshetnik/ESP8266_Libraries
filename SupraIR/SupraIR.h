#ifndef SupraIR_h
#define SupraIR_h

#include <Arduino.h>

#define POWER_OFF	0
#define POWER_ON	1

#define ECO_OFF		0
#define ECO_ON		1

#define NIGHT_OFF	0
#define NIGHT_ON	1

#define MODE_AUTO	0
#define MODE_COOL	1
#define MODE_DRY	2
#define MODE_FAN	3
#define MODE_HEAT	4

#define FAN_AUTO	0
#define FAN_1		1
#define FAN_2		2
#define FAN_3		3

#define VDIR_MANUAL 0
#define VDIR_SWING	1

#define SUPRA_IR_HDR_MARK	9000
#define SUPRA_IR_HDR_SPACE	4500
#define SUPRA_IR_BIT_MARK	600
#define SUPRA_IR_ZERO_SPACE 500
#define SUPRA_IR_ONE_SPACE	1660


class SupraIR
{
	public:
		SupraIR( uint8_t pin, uint8_t frequency = 38 );
		void sendIR( uint8_t powerModeCmd, uint8_t operatingModeCmd, uint8_t fanSpeedCmd, uint8_t temperatureCmd, uint8_t swingVCmd, uint8_t ecoCmd, uint8_t nightCmd );
		void getIR( uint8_t *powerModeCmd, uint8_t *operatingModeCmd, uint8_t *fanSpeedCmd, uint8_t *temperatureCmd, uint8_t *swingVCmd, uint8_t *ecoCmd, uint8_t *nightCmd );

	private:
		void markIR( uint16_t markLength );
		void spaceIR( uint16_t spaceLength );
		void senBytedIR( uint8_t sendByte );

	protected:
		uint8_t _pin;
		uint32_t _halfPeriodicTime;
		uint8_t bytes[13];
};

#endif
