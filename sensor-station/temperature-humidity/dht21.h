/*
 * dht21.h - Library for DHT21/AM2301 digital temperature sensor
 * Create by Jack Tan <jianekemeng@gmail.com>
 * Released into the public domain.
 */

#ifndef	__DHT21_H__
#define	__DHT21_H__

#include "itype.h"

/* DHT21 data pin */
extern uint8_t dht21_read_pin(void);
extern void dht21_write_pin(uint8_t level);
extern void dht21_pin_mode(uint8_t mode);
extern void delay_ms(uint16_t ms);
extern void delay_us(uint16_t us);

/////////////////////////////////////////////////////

uint8_t RH_H, RH_L, T_H, T_L, CRC;

uint8_t dht21_read_8bits(void)
{
	uint8_t i, flag, data = 0;
	uint8_t temp;

	for(i=0; i<8; i++)
	{
		flag = 2;
		temp = 0;

		while((dht21_read_pin() == 0) && flag++);
		delay_us(30);

		if(dht21_read_pin() == 1) temp = 1;

		flag = 2;
		while((dht21_read_pin() == 1) && flag++);

		if(flag == 1) break;

		data <<= 1;
		data |= temp;

	}

	return data;
}

uint8_t dht21_get_data(void)
{
	uint8_t flag;
	dht21_pin_mode(AVR_OUTPUT);

	/* Bus Low, delay 1 ~ 5ms */
	dht21_write_pin(LOW);
	delay_us(1500);

	/* Bus High, delay 40us */
	dht21_write_pin(HIGH);
	delay_us(40);

	dht21_pin_mode(AVR_INPUT);
	if (dht21_read_pin() == 0)
	{
		flag = 2;
		/* waitting the ACK signal (Low, 80us) */
		while ((dht21_read_pin() == 0) && flag++);

		flag = 2;
		/* waitting the DATA start signal (High, 80us) */
		while ((dht21_read_pin() == 1) && flag++);

		RH_H = dht21_read_8bits();
		RH_L = dht21_read_8bits();
		T_H = dht21_read_8bits();
		T_L = dht21_read_8bits();
		CRC = dht21_read_8bits();
		
		dht21_pin_mode(AVR_OUTPUT);
		dht21_write_pin(HIGH);

		return 0;

	}
	else
		return -1;
}

uint8_t dht21_data_check(void)
{
	uint8_t tmp = (T_H + T_L + RH_H + RH_L);
	if(tmp != CRC)
	{
		RH_H = 0;
		RH_L = 0;
		T_H = 0;
		T_L = 0;
		return -1;
	}
	else
		return 0;

}

void dht21_temperature(char *buf)
{
	int16_t T = (T_H << 8) | T_L;
	//float tt;
	int8_t len;

	memset(buf, 0, 8);

	if(T >= 0)
	{
		//tt = T/10 + (T%10) * 0.1;
		sprintf(buf, "%d", T/10);

		len = sizeof(buf);
		buf[len] = '.';
		sprintf(buf + len + 1, "%d", T%10);
	}
	else
	{
		T = T & 0x7FFF;
		//tt = -(T/10 + (T%10) * 0.1);
		buf[0] = '-';

		sprintf(buf + 1, "%d", T/10);

		len = sizeof(buf);
		buf[len] = '.';

		sprintf(buf + len + 1, "%d", T%10);
	}

	//return tt;
}

void dht21_humidity(char *buf)
{
	int16_t RH = (RH_H << 8) | RH_L;
	int8_t len;

	memset(buf, 0, 8);

	//float hum;
	//hum = RH/10 + (RH%10) * 0.1;

	sprintf(buf, "%d", RH/10);

	len = sizeof(buf);
	buf[len] = '.';
	sprintf(buf + len + 1, "%d", RH%10);

	//return hum;
}

#endif
