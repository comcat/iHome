#ifndef	__SLEEP_H__
#define	__SLEEP_H__

#include <iom16v.h>

#define	SLEEP_MODE_IDLE	0
#define	SLEEP_MODE_ADC		(1 << SM0)
#define	SLEEP_MODE_PWR_DOWN	(1 << SM1)
#define	SLEEP_MODE_PWR_SAVE	((1 << SM1) | (1 << SM0))
#define	SLEEP_MODE_STANDBY	((1 << SM2) | (1 << SM1))
#define	SLEEP_MODE_EXT_STANDBY	((1 << SM2) | (1 << SM1) | (1 << SM0))

void set_sleep_mode (uint8_t mode)
{
	uint8_t  mcucr = MCUCR;
	MCUCR = (mcucr & (~mode)) | mode;
}

void sleep_enable(void)
{
	uint8_t  mcucr = MCUCR;
	MCUCR = mcucr | (1 << SE);
}

void sleep_disable(void)
{
	uint8_t  mcucr = MCUCR;
	MCUCR = mcucr & (~(1 << SE));
}

void sleep_mode(void)
{
	asm("SLEEP");
}

#endif
