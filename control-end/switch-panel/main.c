/*
 * Copyright (c) Jack's Lab (www.jackslab.org)
 *
 * Switch Panel with NRF24LE1 
 * Author: Jack Tan <jiankemeng@gmail.com>
 *
 */

#include <intrins.h>
#include <string.h>

#include "reg24le1.h"
#include "nrf24l01.h"
#include "spi.h"
#include "itype.h"
#include "ihome.h"

//#define	SYS_ID			PARLOR_LIGHT
#define SYS_ID		PARLOR_PLANT
#define DEBUG	1

#define	THIS_DEV_ID		0x01

sbit LED1 = P0^2;		// 1 = led on;
sbit LED2 = P0^3;		// 1 = led on; PIN 0.3 also is uart tx pin

sbit swch_in = P0^0;		// Pin 0.0; 1 = High, Switch On
sbit swch_out = P0^1;		// Pin 0.1; 1 = High, Switch On

#define p0dir	0xf1		// 0 is output, 1 is input
#define p1dir	0xff

uint8_t bdata sta;

//sbit RX_DR = sta^6;
//sbit TX_DS = sta^5;
//sbit MAX_RT = sta^4;

/***** delay functions *****/
#if 0
void delay(uint16_t x)
{
    uint16_t i,j;
    i=0;
    for(i=0;i<x;i++)
    {
       j=108;
           ;
       while(j--);
    }
}
#endif

void delay_us(uint16_t us)
{
	do{
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
	} while (--us);
}

void delay_ms(uint16_t ms)
{
	do {
		delay_us(250);
		delay_us(250);
		delay_us(250);
		delay_us(250);
	} while (--ms);
}

/********  SPI API  ********/
char spi_transfer(char value)
{
	SPIRDAT = value;
	while(!(SPIRSTAT & 0x02));  				// 等待SPI传输完成
	return SPIRDAT;             				// 返回读出值
}

void chip_select() { RFCSN = 0; }
void chip_deselect() { RFCSN = 1; }


/*****     NRF24 API   *****/
#define INTERRUPT_RFIRQ		9

#define ID_LEN			5   				// RF收发地址共5 bytes 
#define TX_PLOAD_WIDTH		32  				// 数据包长度为32 bytes

uint8_t sys_id[ID_LEN] = { 0xBF, 0xA3, 0x21, 0x23, 0x01 };	//本地地址

uint8_t data rx_buf[TX_PLOAD_WIDTH];
uint8_t data tx_buf[TX_PLOAD_WIDTH];

void chip_enable() { RFCE = 1; }
void chip_disable() { RFCE = 0; }

char rf_stat;

enum
{
	POWER_DOWN,
	STANDBY_I,
	STANDBY_II,
	TX,
	RX,
	PRE_TX,
	BUSY
};

void nrf24_trans_init(void)
{
	chip_disable();

  	RFCKEN = 1;                                 		// 启动RF时钟
  	RF = 1;                                     		// 允许RF中断
	delay_ms(1);	

  	spi_write_reg(WR_REG + RF_CH, 40);			// RF频率2440MHz
  	spi_write_reg(WR_REG + RF_SETUP, 0x07);   		// 发射功率0dBm, 传输速率2Mbps,
  	spi_write_reg(WR_REG + EN_AA, 0x01);      		// 启动自动应答功能
	spi_write_reg(WR_REG + SETUP_AW, 0x3);		// this should be 0x3

	/* TX config */
  	spi_write_buf(WR_REG + TX_ADDR, sys_id, ID_LEN);    	// 设置发射地址长度
  	spi_write_reg(WR_REG + SETUP_RETR, 0x0a); 		// 自动重传10次
	spi_write_reg(FLUSH_TX, 0);				// 清除TX的FIFO

	/* RX config */
  	spi_write_buf(WR_REG + RX_ADDR_P0, sys_id, ID_LEN); 	// 设置接收地址长度
  	spi_write_reg(WR_REG + RX_PW_P0, TX_PLOAD_WIDTH); 	// PIPE0 接收数据包长度
  	spi_write_reg(WR_REG + EN_RXADDR, 0x01);  		// PIPE0接收数据
	spi_write_reg(FLUSH_RX, 0);				// 清除RX的FIFO

//	spi_write_reg(WR_REG + CONFIG, EN_CRC | CRCO | PWR_UP);		// enter standby-I mode
//	delay_ms(5);

//	rf_stat = STANDBY_I;
}

void nrf24_rx(void)
{
	chip_disable();

	// 上电, CRC为2 bytes,接收模式,允许RX_DR产生中断
  	spi_write_reg(WR_REG + CONFIG, EN_CRC | CRCO | PWR_UP | PRIM_RX);

	chip_enable();
	rf_stat = RX;
	delay_us(150);
}

void nrf24_tx(char *tx_buf, char len)
{
	chip_disable();
	spi_write_buf(WR_TX_PLOAD, tx_buf, len);
	spi_write_reg(WR_REG + CONFIG, EN_CRC | CRCO | PWR_UP);
	chip_enable();

	rf_stat = TX;

	delay_us(150);
}

#ifdef DEBUGX
void no_uart_test()
{
	rf_stat = PRE_TX;
	memcpy(tx_buf, rx_buf, 32);
	delay_ms(100);
}
#endif

uint8_t parse_cmd();

void nrf24_isr(void) interrupt INTERRUPT_RFIRQ
{
//  	EA = 0;                                      		 // 关中断
	chip_disable();

	sta = spi_read_reg(STATUS);				// 读出状态值
	if(sta & RX_DR)
	{
		spi_read_buf(RD_RX_PLOAD, rx_buf, 32);		// 读出FIFO的数据
		parse_cmd();
#ifdef DEBUGX
		no_uart_test();
#endif
		memset(rx_buf, 0, 32);

		spi_write_reg(FLUSH_RX, 0);			// 清除RX的FIFO
		spi_write_reg(WR_REG + STATUS, sta);
		if(rf_stat != PRE_TX)
			nrf24_rx();
	}
	else if(sta & TX_DS)
	{
		spi_write_reg(FLUSH_TX,0);			// 清除TX的FIFO
		spi_write_reg(WR_REG + STATUS, sta);
		nrf24_rx();
	}
	else if(sta & MAX_RT)
	{
		spi_write_reg(FLUSH_TX,0);			// 清除TX的FIFO
		spi_write_reg(WR_REG + STATUS, sta);
		rf_stat = PRE_TX;
	}
//	EA = 1;
}

/*****  Switch related functions *****/
uint8_t switch_stat;

void switch_on(void)
{
	swch_out = 1;
	switch_stat = 1;
	
#ifdef	DEBUG
	LED1 = 1;
	delay_ms(300);
	LED1 = 0;
#endif
}

void switch_off(void)
{
	swch_out = 0;
	switch_stat = 0;
	
#ifdef	DEBUG
	LED2 = 1;
	delay_ms(300);
	LED2 = 0;
#endif
}

void change_switch_stat()
{
	switch_stat = !switch_stat;
	swch_out = switch_stat;
}

uint8_t read_pin_stat(void)
{
	return switch_stat;
}

void return_stat(void)
{
	memset(tx_buf, 0, 32);
	tx_buf[0] = T_DATA;
	tx_buf[1] = CENTRAL_GW;
	tx_buf[2] = 0xFF;
	tx_buf[3] = SYS_ID;
	tx_buf[4] = THIS_DEV_ID;
	tx_buf[5] = 0x01;
	tx_buf[6] = read_pin_stat(); 

	rf_stat = PRE_TX;
}

void scan_ack()
{
	memset(tx_buf, 0, 32);
	tx_buf[0] = T_ACK;
	tx_buf[1] = CENTRAL_GW;
	tx_buf[2] = 0xFF;
	tx_buf[3] = SYS_ID;
	tx_buf[4] = THIS_DEV_ID;

	rf_stat = PRE_TX;
}

void forward_packet()
{
	int fc = rx_buf[5];
	uint8_t *p = rx_buf;
	uint8_t *d = tx_buf;

	memset(tx_buf, 0, 32);

	if(fc == 2)
	{
		tx_buf[0] = T_FORWARD;
		tx_buf[1] = rx_buf[3];
		tx_buf[2] = rx_buf[4];
		tx_buf[5] = 1;
		memcpy(d+6, p+6, 26);
	}
	else if(fc == 1)
	{
		memcpy(d, p+6, 26);
	}

	rf_stat = PRE_TX;
}

uint8_t parse_cmd()
{
	if(rx_buf[1] != SYS_ID)
		return -2;	//ignore

	if(rx_buf[2] == 0xFF || rx_buf[2] == THIS_DEV_ID)
	{
		if(rx_buf[0] == T_CMD)
		{
			switch(rx_buf[6])
			{
				case SET_SWITCH_ON:
					switch_on();
					break;
				case SET_SWITCH_OFF:
					switch_off();
					break;
				case QUERY_SWITCH_STAT:
					return_stat();
					break;
				case SCAN_DEV:
					scan_ack();
					break;
				default:
					return -1;
			}
		}
		else if(rx_buf[0] == T_FORWARD)
		{
			forward_packet();
		}
	}

	return 0;
}

#ifdef	DEBUGX
void uart_init(void)
{
	ES0 = 0;                      			// 关UART0中断
	REN0 = 1;                     			// 允许接收
	SM0 = 0;                      			// 串口模式1，8bit可变波特率
	SM1 = 1;                   
	PCON |= 0x80;                 			// SMOD = 1
	ADCON |= 0x80;                			// 选择内部波特率发生器

	S0RELL = 0xe6;					// 波特率19.2K(十进制998=十六进制0x03e6)
	S0RELH = 0x03;
	TI0 = 0;					// 清发送完成标志
	S0BUF=0x99;					// 送初值
}

void uart_putchar(uint8_t x)
{
	while (!TI0);					// 等待发送完成
	TI0=0;						// 清发送完成标志
	S0BUF=x;					// 发送数据
}
#endif

void io_init(void)
{
  	P0DIR = p0dir;	   	// 设定I/O口输入输出
  	P1DIR = p1dir;					

	switch_stat = 0;
	swch_out = 0;		// 来电重启，开关默认关闭

#ifdef	DEBUG
	LED1=LED2=1;		// 灯全亮
	delay_ms(200);
	LED1=LED2=0;		// 灯全灭                  
	delay_ms(200);
	LED1=LED2=1;		// 灯全亮
	delay_ms(200);
	LED1=LED2=0;		// 灯全灭                  
#endif
}  

void main(void)
{
	io_init();					// I/O口初始化
#ifdef	DEBUGX
	uart_init();                      		// 串口初始化 
#endif

	nrf24_trans_init();				// RF初始化                            
  	EA=1;						// 允许中断

	nrf24_rx();					//进入接收模式

	while(1)
	{
		if(swch_in == 1)
		{
			change_switch_stat();
			delay_ms(1000);
		}
		if(rf_stat == PRE_TX)
		{
			nrf24_tx(tx_buf, 32);
		}
	}	
}
