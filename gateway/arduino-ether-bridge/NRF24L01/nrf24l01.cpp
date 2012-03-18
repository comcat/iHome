/*
 * nrf24l01.cpp - Library for NRF24L01 2.4GHz Transceiver
 * Created by Jack Tan <jiankemeng@gmail.com>
 * Released into the public domain.
 */

#include "nrf24l01.h"

NRF24L01::NRF24L01(byte csn, byte ce, byte irq)
{
	_pin_CSN = csn;
	_pin_CE = ce;
	_pin_IRQ = irq;

	_pin_SCK = 9;
	_pin_MISO = 8;
	_pin_MOSI = 4;

	pinMode(_pin_CE, OUTPUT);
	pinMode(_pin_IRQ, INPUT);

	// enable upload resistor
	digitalWrite(_pin_IRQ, HIGH); 

	//spi_begin();
	pinMode(_pin_SCK, OUTPUT);
	pinMode(_pin_MISO, INPUT);
	pinMode(_pin_MOSI, OUTPUT);
	pinMode(_pin_CSN, OUTPUT);

	digitalWrite(_pin_SCK, LOW); 
	digitalWrite(_pin_MOSI, LOW); 
	digitalWrite(_pin_CSN, HIGH); 
	// enable upload resistor
	digitalWrite(_pin_MISO, HIGH); 
}

NRF24L01::NRF24L01(void)
{
	_pin_SCK = 9;
	_pin_MISO = 8;
	_pin_MOSI = 4;

	_pin_CSN = 7;
	_pin_CE = 6;
	_pin_IRQ = 5;

	pinMode(_pin_CE, OUTPUT);
	pinMode(_pin_IRQ, INPUT);

	// enable upload resistor
	digitalWrite(_pin_IRQ, HIGH); 

	//spi_begin();
	pinMode(_pin_SCK, OUTPUT);
	pinMode(_pin_MISO, INPUT);
	pinMode(_pin_MOSI, OUTPUT);
	pinMode(_pin_CSN, OUTPUT);

	digitalWrite(_pin_SCK, LOW); 
	digitalWrite(_pin_MOSI, LOW); 
	digitalWrite(_pin_CSN, HIGH); 
	// enable upload resistor
	digitalWrite(_pin_MISO, HIGH); 
}

void NRF24L01::chip_select()
{
	// disable interrupt
	//cli();

	// set the SS to LOW, enable the chip
	digitalWrite(_pin_CSN, LOW);
}

void NRF24L01::chip_deselect()
{
	// set the SS to HIGH, disable the chip
	digitalWrite(_pin_CSN, HIGH);

	// enaable interrupt
	//sei();
}

byte NRF24L01::spi_transfer(byte data)
{
	byte i, temp = 0;
	for(i=0; i<8; i++)
	{
		if(data & 0x80)
			digitalWrite(_pin_MOSI, HIGH);
		else
			digitalWrite(_pin_MOSI, LOW);

		data = (data << 1);
		temp <<= 1;

		digitalWrite(_pin_SCK, HIGH);

		if(digitalRead(_pin_MISO))
			temp++;

		digitalWrite(_pin_SCK, LOW);
	}

	return (temp);
}


byte NRF24L01::read_reg(byte addr)
{
	byte value;
	chip_select();
	spi_transfer(addr+RD_REG); //select register to read from
	value = spi_transfer(0); // read the register value
	chip_deselect();
	return value;
}

byte NRF24L01::write_reg(byte addr, byte value)
{
	byte status;
	//addr += WR_REG;
	chip_select();
	status = spi_transfer(addr); //select register
	spi_transfer(value); //write value to it
	chip_deselect();
	return status;
}

byte NRF24L01::read_buf(byte addr, byte *p_buf, byte n_bytes)
{
	byte status, i;
	chip_select();
	status = spi_transfer(addr); // select the buffer register
	for (i=0; i<n_bytes; i++)
		p_buf[i] = spi_transfer(0);

	chip_deselect();
	return status;
}

byte NRF24L01::write_buf(byte addr, byte *p_buf, byte n_bytes)
{
	byte status, i;
	chip_select();
	status = spi_transfer(addr); // select the buffer register
	for (i=0; i<n_bytes; i++)
		spi_transfer(*p_buf++);
	chip_deselect();
	return status;

}

/* some common NRF24L01 register config (tx and rx) */
void NRF24L01::trans_init()
{
	digitalWrite(_pin_CE, LOW);	// 24L01 chip disable, Power Down Mode

	write_reg(WR_REG+RF_CH, 40);		// set the RF channel to 2.4GHz
	write_reg(WR_REG+RF_SETUP, 0x07);	// set the PA to 0dBm (highest power, 11.3mA current)
	write_reg(WR_REG+EN_AA, 0x01);		// enable auto ack:pipe0
	write_reg(WR_REG+SETUP_AW, 0x3);

//	write_reg(WR_REG+CONFIG, PWR_UP);	// enter standby-I mode
//	delay(5);
}

void NRF24L01::rx_config(byte pipe_num, byte *pipe_addr, byte pipe_addr_width, byte rx_pload_width)
{
	byte tmp;

	digitalWrite(_pin_CE, LOW); // 24L01 chip disable, Power Down Mode
	write_buf(WR_REG + RX_ADDR_P0 + pipe_num, pipe_addr, pipe_addr_width);
	write_reg(WR_REG+RX_PW_P0 + pipe_num, rx_pload_width);

	tmp = read_reg(EN_RXADDR) | (1 << pipe_num);
	write_reg(WR_REG+EN_RXADDR, tmp);
}

void NRF24L01::tx_config(byte *tx_addr, byte tx_addr_width)
{
	digitalWrite(_pin_CE, LOW); // 24L01 chip disable, Power Down Mode
	write_buf(WR_REG + TX_ADDR, tx_addr, tx_addr_width);
	write_buf(WR_REG + RX_ADDR_P0, tx_addr, tx_addr_width);
	write_reg(WR_REG+SETUP_RETR, 0x0a);	// 250us + 86us, 10 re-transmit on fail of AA
}

void NRF24L01::rx()
{
	digitalWrite(_pin_CE, LOW); // 24L01 chip disable, Power Down Mode
	write_reg(WR_REG+CONFIG, (EN_CRC | CRCO | PWR_UP | PRIM_RX));

	digitalWrite(_pin_CE, HIGH); // 24L01 chip enable + PWR_UP(1) + PRIM_RX = RX mode
	delayMicroseconds(150);
}

void NRF24L01::tx(byte *tx_data_buf, byte data_len)
{
	digitalWrite(_pin_CE, LOW); // 24L01 chip disable, Power Down Mode

	write_buf(WR_TX_PLOAD, tx_data_buf, data_len);
	write_reg(WR_REG+CONFIG, 0x0e);

	digitalWrite(_pin_CE, HIGH); // trigger tx
	delayMicroseconds(150);
}


byte NRF24L01::check_tx_flag()
{
	byte tmp, flag;

	/* if transmit successfully or up to max number of re-transmit, IRQ is low */
	while(digitalRead(_pin_IRQ));

	digitalWrite(_pin_CE, LOW);

	tmp = read_reg(STATUS);
	if(tmp & TX_DS) // trans successfully
	{
		flag = 1;
	}
	else if(tmp & MAX_RT)
	{
		flag = -2;
	}
	else
		flag = 0;

	write_reg(FLUSH_TX, 0);
	write_reg(WR_REG+STATUS, tmp);  // clear TX_DS or MAX_RT interrupt

	return tmp;
}


byte NRF24L01::check_rx(byte *buf, byte rx_pload_width)
{
	byte tmp;

	while(digitalRead(_pin_IRQ));

	digitalWrite(_pin_CE, LOW);

	tmp = read_reg(STATUS);
	if(tmp & RX_DR)				// received data
	{
		memset(buf, 0, 32);
		read_buf(RD_RX_PLOAD, buf, rx_pload_width);
		write_reg(WR_REG+STATUS, tmp);		//clear RX_DR interrupt flag
		return 1;
	}
	else
		return 0;			// no data received
}
