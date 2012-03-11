/*
 * Copyright (c) Jack's Lab (www.jackslab.org)
 * Author: Jack Tan <jiankemeng@gmail.com>
*/

/* hardware related function */
void spi_init(void);
char spi_transfer(char);
void chip_select(void);
void chip_deselect(void);

/////////////////////////////////////////////////////////////////////////////////
// NRF24LE1 C51 style
/*
char spi_transfer(char data)
{
	SPIRDAT = data;
	while(!(SPIRSTAT & 0x02));
	return SPIRDAT;
}

void chip_select() { RFCSN = 0; }
void chip_deselect() { RFCSN = 1; }
*/

/////////////////////////////////////////////////////////////////////////////////
// STC C51 style
//
/*
sbit CSN = P1^7
sbit MOSI = P1^5
sbit MISO = P1^3
sbit SCK = P1^4

void spi_init()
{
	CSN = 1;
	SCK = 0;
}

char spi_transfer(char data)
{
	char i;
	for(i=0; i<8; i++)
	{
		MOSI = (data & 0x80);
		data = (data << 1);
		SCK = 1;
		data |= MISO;
		SCK = 0;
	}

	return (data);
}

void chip_select() { CSN = 0; }
void chip_deselect() { CSN = 1; }
*/

/////////////////////////////////////////////////////////////////////////////////
// AVR style
//
/*
#define SET_MOSI_HIGH	PORTB |= (1 << PB5)
#define SET_MOSI_LOW	PORTB &= ~(1 << PB5)
#define SET_SCK_HIGH	PORTB |= (1 << PB7)
#define SET_SCK_LOW	PORTB &= ~(1 << PB7)
#define SET_CSN_HIGH	PORTB |= (1 << PB4)
#define SET_CSN_LOW	PORTB &= ~(1 << PB4)
#define GET_MISO	PINB & (1 << PB6)

void spi_init()
{
	SET_CSN_HIGH;
	SET_SCK_LOW;
	SET_MOSI_LOW;
}

char spi_transfer(char data)
{
	char i, temp = 0;
	for(i=0; i<8; i++)
	{
		if(data & 0x80)
			SET_MOSI_HIGH;
		else
			SET_MOSI_LOW;
		data = (data << 1);
		temp <<= 1;

		SET_SCK_HIGH;

		if(GET_MISO)
			temp++;

		SET_SCK_LOW;
	}

	return (temp);
}

void chip_select() { SET_CSN_LOW; }
void chip_deselect() { SET_CSN_HIGH; }
*/

/* following is independent function */
char spi_read_reg(char reg)
{
	char reg_val;
	chip_select();

	spi_transfer(reg);
	reg_val = spi_transfer(0);

	chip_deselect();
	return (reg_val);
}

char spi_write_reg(char reg, char val)
{
	char status;
	chip_select();

	status = spi_transfer(reg);
	spi_transfer(val);

	chip_deselect();
	return (status);
}

char spi_read_buf(char reg, char *pbuf, char uchar)
{
	char status, i;
	chip_select();

	status = spi_transfer(reg);
	for(i=0; i<uchar; i++)
		pbuf[i] = spi_transfer(0);

	chip_deselect();

	return (status);
}

char spi_write_buf(char reg, char *pbuf, char uchar)
{
	char status, i;
	chip_select();
	status = spi_transfer(reg);
	for(i=0; i<uchar; i++)
		spi_transfer(*pbuf++);

	chip_deselect();

	return (status);
}


