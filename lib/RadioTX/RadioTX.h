/*
 * RadioTX.h - Library for Simple 315MHz Radio module
 * Created by Jack Tan <jiankemeng@gmail.com>
 * Released into the public domain.
 */

#ifndef __RADIOTX_H__

#include <Arduino.h>

class RadioTX
{
	public:
		RadioTX(uint8_t pin = 8, uint16_t ick = 242);
		void issue(uint32_t code);
		void issue(uint32_t code, uint16_t ck);
		void set_ick(uint16_t ck) { _ick = ck; }
		uint16_t get_ick() { return _ick; }

	private:
		// 4 ick is a data cycle (3 high + 1 low or 1 high + 3 low) 
		uint16_t _ick;
		uint8_t _tx_pin;
		void sync();
		void _0();
		void _1();
};

#endif
