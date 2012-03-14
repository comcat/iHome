#include <iom16v.h>
#include <macros.h>
#include <string.h>
#include <stdio.h>

#include "ihome.h"
#include "itype.h"
#include "spi.h"
#include "nrf24l01.h"

#include "dht21.h"

void Delay(int s)
{
	unsigned int i;
	for (i = 0; i < s; i++);
	for (i = 0; i < s; i++);
}

/* 1 is 4us, 10 is about 14us */
void delay_us(uint16_t us)
{
	//uint8_t i;
	//for(i=0; i<us; i++);
	while(--us);
}

/* 1 is about 6ms, 10 is about 60ms */
void delay_ms(uint16_t ms)
{
	uint16_t i, j;
	for (i = 0; i < (unsigned int) (8 * 143 - 2) * ms; i++);
}

void delay(int s)
{
	char i;
	for (i = 0; i < s; i++)
		delay_ms(200);
}


//-------------------------------LED指示灯--------------------------------------
//#define LED1          PB3     //端口为输出状态
#define    High_LED1        PORTB |=  (1 << PB3)
#define    Low_LED1        PORTB &= ~(1 << PB3)
//#define LED2          PD3     //端口为输出状态
#define    High_LED2        PORTD |= (1 << PD3)
#define    Low_LED2        PORTD &= ~(1 << PD3)

//---------------------------- NRF24L01 API ------------------------------------
void nrf24_trans_init(void);
void nrf24_rx_config(char pipe_num, char *pipe_addr,
		 char pipe_addr_width, char rx_pload_width);
void nrf24_tx_config(char *tx_addr, char tx_addr_width);
void nrf24_rx(void);
void nrf24_tx(char *tx_buf, char len);
char nrf24_check_rx(char *buf, char rx_pload_width);
char nrf24_check_tx_flag(void);
char nrf24_get_rx_data(char *buf, char rx_pload_width);

#define   Low_24L01_CSN		PORTB &= ~(1 << PB4)
#define   High_24L01_CSN	PORTB |=  (1 << PB4)

#define   High_24L01_MOSI	PORTB |=  (1 << PB5)
#define   Low_24L01_MOSI	PORTB &= ~(1 << PB5)
#define   Read_24L01_MOSI	PINB  &   (1 << PB5)

#define   High_24L01_MISO	PORTB |=  (1 << PB6)
#define   Low_24L01_MISO	PORTB &= ~(1 << PB6)
#define   Read_24L01_MISO	PINB  &   (1 << PB6)

#define   High_24L01_SCK	PORTB |=  (1 << PB7)
#define   Low_24L01_SCK		PORTB &= ~(1 << PB7)
#define   Read_24L01_SCK	PINB  &   (1 << PB7);

#define   High_24L01_CE		PORTD |=  (1 << PD4)
#define   Low_24L01_CE		PORTD &= ~(1 << PD4)
#define   Read_24L01_CE		PIND  &   (1 << PD4)

#define   High_NRF24L01_IRQ	PORTD |=  (1 << PD2)
#define   Low_NRF24L01_IRQ	PORTD &= ~(1 << PD2)
#define   Read_NRF24L01_IRQ	PIND  &   (1 << PD2)

#define    CHIP_ENABLE		High_24L01_CE
#define    CHIP_DISABLE		Low_24L01_CE
#define    NRF24_IRQ		Read_NRF24L01_IRQ

#define ID_LEN    5		// 5 uints RX address width
#define TX_PLOAD_WIDTH  32
#define RX_PLOAD_WIDTH  32

//-------------------------------- SPI API -------------------------------------
#define    SET_MOSI_HIGH    High_24L01_MOSI
#define	   SET_MOSI_LOW    Low_24L01_MOSI
#define    SET_SCK_HIGH    High_24L01_SCK
#define    SET_SCK_LOW    Low_24L01_SCK
#define	   MISO_BIT    (PINB & 0x40)
#define    SET_CSN_HIGH    High_24L01_CSN
#define    SET_CSN_LOW    Low_24L01_CSN


//////////////////////////////////////////////////////////////////////////////////

void spi_init()
{
	SET_CSN_HIGH;				//spi disable
	SET_SCK_LOW;				//clock line init high
	SET_MOSI_LOW;
}

char spi_transfer(char x_data)
{
	char i, temp = 0;
	for (i = 0; i < 8; i++)		// output 8-bit
	{
		if (x_data & 0x80) {
			SET_MOSI_HIGH;		// output 'uchar', MSB to MOSI
		}

		else {
			SET_MOSI_LOW;
		}
		x_data = (x_data << 1);	// shift next bit into MSB..
		temp <<= 1;

		SET_SCK_HIGH;			// Set SCK high..

		if (MISO_BIT)
			temp++;				// capture current MISO bit

		SET_SCK_LOW;			// ..then set SCK low again
	}
	return (temp);				// return read uchar
}

void chip_select()
{
	SET_CSN_LOW;
}

void chip_deselect()
{
	SET_CSN_HIGH;
}


//////////////////////////////////////////////////////////////////////////////////


/* ihome systom identification number */
char sys_id[ID_LEN] = { 0xBF, 0xA3, 0x21, 0x23, 0x01 };

char tx_buf[32], rx_buf[32];

uint8_t rf_stat;

enum
{
	POWER_DOWN,
	STANDBY_I,
	STANDBY_II,
	IN_TX,
	IN_RX,
	PRE_TX,
	BUSY
};

void nrf24_trans_init(void)
{
	CHIP_DISABLE;				// 24L01 chip disable, Power down Mode
	spi_write_reg(WRITE_REG + RF_CH, 40);	//设置信道工作为2.4GHZ，收发必须一致
	spi_write_reg(WRITE_REG + RF_SETUP, 0x07);	//设置发射速率为1MHZ，发射功率为最大值0dB
	spi_write_reg(WRITE_REG + EN_AA, 0x01);	//pipe0-5自动   ACK应答允许 
	spi_write_reg(WRITE_REG + SETUP_AW, 0x3);

	spi_write_reg(WRITE_REG + CONFIG, PWR_UP);	// enter standby-I mode

	Delay(6000);

	rf_stat = STANDBY_I;
}

void nrf24_rx_config(char pipe_num, char *pipe_addr,
					 char pipe_addr_width, char rx_pload_width)
{
	char tmp;
	CHIP_DISABLE;				// 24L01 chip disable, Power Down Mode
	spi_write_buf(WRITE_REG + RX_ADDR_P0 + pipe_num, pipe_addr,
				  pipe_addr_width);
	spi_write_reg(WRITE_REG + RX_PW_P0 + pipe_num, rx_pload_width);	//rx data pool width
	//spi_write_reg(WRITE_REG + EN_RXADDR, 0x01);           //enable rx pipe 0
	tmp = spi_read_reg(EN_RXADDR) | (1 << pipe_num);
	spi_write_reg(WRITE_REG + EN_RXADDR, tmp);
}

void nrf24_tx_config(char *tx_addr, char tx_addr_width)
{
	CHIP_DISABLE;
	spi_write_buf(WRITE_REG + TX_ADDR, tx_addr, tx_addr_width);
	spi_write_buf(WRITE_REG + RX_ADDR_P0, tx_addr, tx_addr_width);	// for auto aa
	spi_write_reg(WRITE_REG + SETUP_RETR, 0x0a);	//250us + 86us, 10 re-transmit on fails of AA
}

void nrf24_rx(void)
{
	CHIP_DISABLE;
	spi_write_reg(WRITE_REG + CONFIG, (EN_CRC | CRCO | PWR_UP | PRIM_RX));
	//spi_write_reg(WRITE_REG + CONFIG, 0x0F);
	CHIP_ENABLE;

	rf_stat = IN_RX;

	Delay(930);
}

void nrf24_power_down(void)
{
	char tmp;
	CHIP_DISABLE;
	tmp = spi_read_reg(CONFIG);
	tmp &= ~PWR_UP;
	spi_write_reg(WRITE_REG + CONFIG, tmp);

	rf_stat = POWER_DOWN;
}

void nrf24_tx(char *tx_buf, char len)
{
	CHIP_DISABLE;
	spi_write_buf(WR_TX_PLOAD, tx_buf, len);
	spi_write_reg(WRITE_REG + CONFIG, 0x0E);	// IRQ收发完成中断响应，16位CRC，主发送
	CHIP_ENABLE;

	rf_stat = IN_TX;

	Delay(930);
}

char nrf24_check_rx(char *buf, char rx_pload_width)
{
	char st, flag;

	while (NRF24_IRQ);

	CHIP_DISABLE;				// enter standby-I mode
	st = spi_read_reg(STATUS);
	if (st & RX_DR) {
		//tmp = spi_read_reg(STATUS);
		//pipe_num = (tmp & RX_P_NO) >> 1;
		spi_read_buf(RD_RX_PLOAD, buf, rx_pload_width);
		flag = 1;
	}
	else
		flag = 0;

	spi_write_reg(WRITE_REG + STATUS, st);	//clear interrupt flag
	return flag;
}


/******************** DHT21 function ******************/
// Using PA7 as data pin

uint8_t dht21_read_pin(void)
{
	if(PINA & (1 << PA7))
		return 1;
	else
		return 0;
}

void dht21_write_pin(uint8_t level)
{
	if(level == HIGH)
		PORTA |= (1 << PA7);
	else if(level == LOW)
		PORTA &= ~(1 << PA7);
}

void dht21_pin_mode(uint8_t mode)
{
	if(mode == AVR_OUTPUT)
	{
		DDRA |= (1 << PA7);	// PA7 Out
		DDRA = DDRA & 0xff;                                                       
	}
	else if(mode == AVR_INPUT)
	{
		DDRA &= ~(1 << PA7);
		DDRA = DDRA & 0xff;                                                       
	}
}

////////////////////////////////////////////////////////
#define	THIS_DEV_ID	PARLOR_TEMP_HUM

void read_data(void)
{
	dht21_get_data();
}

void return_data(void)
{
	memset(tx_buf, 0, 32);
	tx_buf[0] = T_DATA;
	tx_buf[1] = CENTRAL_GW;
	tx_buf[2] = 0xFF;
	tx_buf[3] = PARLOR_PLANT;
	tx_buf[4] = THIS_DEV_ID;
	tx_buf[5] = 0x4;

	tx_buf[6] = T_H;
	tx_buf[7] = T_L;
	tx_buf[8] = RH_H;
	tx_buf[9] = RH_L;

	rf_stat = PRE_TX;
}

void scan_ack()
{
	memset(tx_buf, 0, 32);
	tx_buf[0] = T_ACK;
	tx_buf[1] = CENTRAL_GW;
	tx_buf[2] = 0xFF;
	tx_buf[3] = AIR_SENSOR;
	tx_buf[4] = THIS_DEV_ID;

	rf_stat = PRE_TX;
}

void forward_packet()
{
	int fc = rx_buf[5];
	uint8_t *p = rx_buf;
	uint8_t *d = tx_buf;

	memset(tx_buf, 0, 32);

	if (fc == 2) {
		tx_buf[0] = T_FORWARD;
		tx_buf[1] = rx_buf[3];
		tx_buf[2] = rx_buf[4];
		tx_buf[5] = 1;
		memcpy(d + 6, p + 6, 26);
	}
	else if (fc == 1) {
		memcpy(d, p + 6, 26);
	}

	rf_stat = PRE_TX;
}

uint8_t parse_cmd(void)
{
	//check sys, ignor dev id
	if (rx_buf[1] != AIR_SENSOR) {
		//it's not for this device
		return 0;
	}

	if (rx_buf[2] == 0xFF || rx_buf[2] == THIS_DEV_ID) {
		if (rx_buf[0] == T_CMD) {
			switch (rx_buf[6]) {
				case PREFETCH_DATA:
					read_data();
					break;
				case GET_DATA:
					return_data();
					break;
				case SCAN_DEV:
					scan_ack();
					break;
				default:
					return -1;
			}
		}
		else if (rx_buf[0] == T_FORWARD) {
			forward_packet();
		}
	}

	return 0;
}

//#pragma interrupt_handler nrf24_isr:2
void nrf24_isr(void)
{
	char st;

	CHIP_DISABLE;				// enter standby-I mode

	rf_stat = BUSY;

#ifdef	NO_UART_DEBUG
	High_LED1;
	delay_ms(300);
	Low_LED1;
	delay_ms(300);
	High_LED1;
#endif

	st = spi_read_reg(STATUS);
	if (st & RX_DR) {
		spi_read_buf(RD_RX_PLOAD, rx_buf, 32);
		parse_cmd();

		//clear rx_buf
		memset(rx_buf, 0, 32);
		spi_write_reg(FLUSH_RX, 0);	// clear tx fifo, enter standby-I mode
		spi_write_reg(WRITE_REG + STATUS, st);	//clear interrupt flag

		if (rf_stat != PRE_TX)
			nrf24_rx();			// rx
	}
	else if (st & TX_DS) {
		spi_write_reg(FLUSH_TX, 0);	// clear tx fifo, enter standby-I mode
		spi_write_reg(WRITE_REG + STATUS, st);	//clear interrupt flag

		nrf24_rx();
	}
	else if (st & MAX_RT) {
		spi_write_reg(FLUSH_TX, 0);	// clear tx fifo, enter standby-I mode
		spi_write_reg(WRITE_REG + STATUS, st);	//clear interrupt flag
		rf_stat = PRE_TX;
	}

}


void main(void)
{
	char i;
	DDRA = DDRA | 0x80;			// PA7 Out
	DDRA = DDRA & 0xff;			//端口方向设置                                                                       
	DDRB = DDRB | 0xB8;
	DDRB = DDRB & 0xBF;

	DDRC = DDRC & 0x7f;

	DDRD = DDRD | 0x1A;	//PD0(rx) input, PD1(tx) output
				//PD2 input, PD3, PD4 output
	DDRD = DDRD & 0xFA;

	PORTD = 0xff;

	SEI();

	Low_LED1;
	Low_LED2;

	delay_ms(1000);

	High_LED1;
	High_LED2;


	spi_init();

	nrf24_trans_init();
	nrf24_tx_config(sys_id, 5);
	nrf24_rx_config(0, sys_id, 5, 32);

	nrf24_rx();

	while (1) {
		while (NRF24_IRQ);
		nrf24_isr();
		if (rf_stat == PRE_TX) {
			nrf24_tx(tx_buf, 32);
		}
	}
}
