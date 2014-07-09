/*
 * Library for Simple 315MHz Radio module
 * Created by Jack Tan <jiankemeng@gmail.com>
 * Released into the public domain.
 */

#include "RadioTX.h"

RadioTX::RadioTX(uint8_t pin, uint16_t ick)
{
	_ick = ick;
	_tx_pin = pin;
}

void RadioTX::sync()
{
	digitalWrite(_tx_pin, LOW);
	digitalWrite(_tx_pin, HIGH);
	delayMicroseconds(_ick);
	digitalWrite(_tx_pin, LOW);
	delayMicroseconds(31*_ick);
}


void RadioTX::_1()
{
	digitalWrite(_tx_pin, HIGH);
	delayMicroseconds(3*_ick);
	digitalWrite(_tx_pin, LOW);
	delayMicroseconds(_ick);
}

void RadioTX::_0()
{
	digitalWrite(_tx_pin, HIGH);
	delayMicroseconds(_ick);
	digitalWrite(_tx_pin, LOW);
	delayMicroseconds(3*_ick);
}

// 0x9f2f14
void RadioTX::issue(uint32_t code)
{
	int i = 23;

	sync();

	for(i=23; i>=0; i--)
	{

		switch ((code >> i) & 0x1) {

			case 1:
				RadioTX::_1();
				break;

			case 0:
				RadioTX::_0();
				break;
		}

	} 

	sync();
}

void RadioTX::issue(uint32_t code, uint16_t ck)
{
	uint8_t i = 23;
	uint16_t tmp = _ick;
	_ick = ck;

	sync();

	for(i=23; i>=0; i--)
	{

		switch ((code >> i) & 0x1) {

			case 1:
				RadioTX::_1();
				break;

			case 0:
				RadioTX::_0();
				break;
		}

	} 

	sync();

	_ick = tmp;
}
