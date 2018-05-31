#include "SupraIR.h"

SupraIR::SupraIR( uint8_t pin, uint8_t frequency )
{
	_pin = pin;
	pinMode( pin, OUTPUT);
	if( frequency == 0 || frequency > 56 ) frequency = 38;
	_halfPeriodicTime = 500UL/((uint32_t)frequency); // T = 1/f but we need T/2 in microsecond and f is in kHz
	bytes[0] = 0xC3; // 0 - header
	bytes[1] = 0x77; // 1 - temperature & vertical swing (default off) 70 - off
	bytes[2] = 0xE0; // 2 - time
	bytes[3] = 0x00; // 3 - minute
	bytes[4] = 0x60; // 4 - fan (default 1) 
	bytes[5] = 0x00; // 5 - turbo (default off)
	bytes[6] = 0x00; // 6 - mode (default auto) 0xX4 - Night
	bytes[7] = 0x00; // 7 - 
	bytes[8] = 0x00; // 8 - 
	bytes[9] = 0x00; // 9 - power 0x2X (default off) 0xX2 - eco
	bytes[10] = 0x1F; // 10 - 
	bytes[11] = 0x05; // 11 - 
	//bytes[12] = 0x00; // 12 - summ
}

void SupraIR::markIR( uint16_t markLength )
{
	uint16_t beginning = (uint16_t)micros();
	while( (uint16_t)(micros() - beginning) < markLength )
	{
		digitalWrite( _pin, HIGH );
		delayMicroseconds( _halfPeriodicTime );
		digitalWrite( _pin, LOW );
		delayMicroseconds( _halfPeriodicTime ); //38 kHz -> T = 26.31 microsec (periodic time), half of it is 13
	}
}

void SupraIR::spaceIR( uint16_t spaceLength )
{
	digitalWrite( _pin, LOW );
	delayMicroseconds( (uint32_t)spaceLength );
}

void SupraIR::senBytedIR( uint8_t sendByte )
{
	for (int i=0; i<8 ; i++)
	{
		if( sendByte & 0x01 )
		{
			markIR(SUPRA_IR_BIT_MARK);
			spaceIR(SUPRA_IR_ONE_SPACE);
		}
		else
		{
			markIR(SUPRA_IR_BIT_MARK);
			spaceIR(SUPRA_IR_ZERO_SPACE);
		}
	sendByte >>= 1;
	}
}

void SupraIR::getIR( uint8_t *powerModeCmd, uint8_t *operatingModeCmd, uint8_t *fanSpeedCmd, uint8_t *temperatureCmd, uint8_t *swingVCmd, uint8_t *ecoCmd, uint8_t *nightCmd )
{
	*powerModeCmd = ( bytes[9] & 0x20 ) ? POWER_ON : POWER_OFF;
	*ecoCmd = ( bytes[9] & 0x02 ) ? ECO_ON : ECO_OFF;
	switch( bytes[6] & 0xF0 )
	{
		case 0x20: *operatingModeCmd = MODE_COOL; break;
		case 0x40: *operatingModeCmd = MODE_DRY; break;
		case 0x80: *operatingModeCmd = MODE_HEAT; break;
		case 0xC0: *operatingModeCmd = MODE_FAN; break;
		default:
		case 0x00: *operatingModeCmd = MODE_AUTO; break;
	}
	*nightCmd = ( bytes[6] & 0x04 ) ? NIGHT_ON : NIGHT_OFF;

	switch( bytes[4] )
	{
		case 0x20: *fanSpeedCmd = FAN_3; break;
		case 0x40: *fanSpeedCmd = FAN_2; break;
		case 0x60: *fanSpeedCmd = FAN_1; break;
		default:
		case 0xA0: *fanSpeedCmd = FAN_AUTO; break;
	}
	*temperatureCmd = ( bytes[1] >> 3 ) + 8;
	*swingVCmd = ( bytes[1] & 0x07 ) ? VDIR_SWING : VDIR_MANUAL;
}

/*
0  1  2  3  4  5  6  7  8  9  10 11 12
C3,77,E2,21,40,00,20,00,00,20,1F,05,E1
C3,77,E2,21,40,00,20,00,00,00,1F,05,C1

C3,77,E4,11,40,00,20,00,00,20,1F,05,D3
C3,77,E4,11,40,00,20,00,00,00,1F,05,B3
*/
void SupraIR::sendIR( uint8_t powerModeCmd, uint8_t operatingModeCmd, uint8_t fanSpeedCmd, uint8_t temperatureCmd, uint8_t swingVCmd, uint8_t ecoCmd, uint8_t nightCmd )
{
	if( powerModeCmd ) bytes[9] = 0x20;
	else bytes[9] = 0x00;
	
	if( ecoCmd ) bytes[9] |= 0x02;

	switch( operatingModeCmd ) 
	{
		case MODE_COOL: bytes[6] = 0x20; break;
		case MODE_DRY: bytes[6] = 0x40; break;
		case MODE_FAN: bytes[6] = 0xC0; break;
		case MODE_HEAT:	bytes[6] = 0x80; break;
		case MODE_AUTO:
		default: bytes[6] = 0x00; break;
	}

	if( nightCmd ) bytes[6] |= 0x04;

    switch( fanSpeedCmd )
	{
		case FAN_1: bytes[4] = 0x60; break;
		case FAN_2: bytes[4] = 0x40; break;
		case FAN_3: bytes[4] = 0x20; break;
		case FAN_AUTO:
		default: bytes[4] = 0xA0; break;
	}

	if( temperatureCmd < 16 ) temperatureCmd = 16;
	else if( temperatureCmd > 32 ) temperatureCmd = 32;
	temperatureCmd -= 8;
	bytes[1] = temperatureCmd << 3;

	if( swingVCmd == VDIR_SWING) bytes[1] &= ~0x07;
	else bytes[1] |= 0x07;

	// calckulating time
	uint16_t _last_time = 60+30+(uint16_t)(millis() / 60000); // minutes
	bytes[3] = (uint8_t)(_last_time % 60);
	bytes[2] |= (uint8_t)( (_last_time / 60 ) % 60 );

	uint8_t checksum = 0x00;
    for( uint8_t i = 0; i < 12; i++ ) checksum += bytes[i];
	bytes[12] = checksum;

	markIR(SUPRA_IR_HDR_MARK);
	spaceIR(SUPRA_IR_HDR_SPACE);

	for( uint8_t i=0; i < 13; i++) senBytedIR ( bytes[i] );

	markIR(SUPRA_IR_BIT_MARK);
	spaceIR(0);
}
