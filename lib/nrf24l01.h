/*
 * Copyright (c) Jack's Lab (www.jackslab.org)
 *
 * nrf24l01.h - Library for NRF24L01 2.4GHz Transceiver
 * Author: Jack Tan <jiankemeng@gmail.com>
 *
 */

#ifndef _NRF24L01_h_
#define _NRF24L01_h_

/* SPI commands */
#define RD_REG		0x00  // Define read command to register
#define WR_REG		0x20  // Define write command to register
#define RD_RX_PLOAD     0x61  // Define RX payload register address
#define WR_TX_PLOAD     0xA0  // Define TX payload register address
#define FLUSH_TX	0xE1  // Define flush TX register command
#define FLUSH_RX	0xE2  // Define flush RX register command
#define REUSE_TX_PL	0xE3  // Define reuse TX payload register command
#define NOP		0xFF  // Define No Operation, might be used to read status register

#define	READ_REG	RD_REG
#define	WRITE_REG	WR_REG


/* nRF24L01 register address */
#define CONFIG          0x00  // 'Config' register address
#define EN_AA           0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR       0x02  // 'Enabled RX addresses' register address
#define SETUP_AW        0x03  // 'Setup address width' register address
#define SETUP_RETR      0x04  // 'Setup Auto. Retrans' register address
#define RF_CH           0x05  // 'RF channel' register address
#define RF_SETUP        0x06  // 'RF setup' register address
#define STATUS          0x07  // 'Status' register address
#define OBSERVE_TX      0x08  // 'Observe TX' register address
#define CD              0x09  // 'Carrier Detect' register address
#define RX_ADDR_P0      0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5      0x0F  // 'RX address pipe5' register address
#define TX_ADDR         0x10  // 'TX address' register address
#define RX_PW_P0        0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1        0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2        0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3        0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4        0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5        0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS     0x17  // 'FIFO Status Register' register address


/* 
 * CONFIG register bits
 */

// 0 - Rx data-ready interrupt is sent to IRQ pin, reset value
// 1 - Interrupt is not sent to IRQ pin
#define	MASK_RX_DR	0x40	

// 0 - Tx data-sent interrupt is sent to IRQ pin, reset value
// 1 - Interrupt is not sent to IRQ pin.
#define MASK_TX_DS	0x20

// 0 - Max-retries-reached interrupt is sent to IRQ pin, reset value
// 1 - Interrupt is not sent to IRQ pin
#define MASK_MAX_RT	0x10

// 0 - Disable automatic CRC
// 1 - Enable automatic CRC, reset value, Forced high if one of the bits in the EN_AA is high
#define EN_CRC		0x08

// 0 - Use 1-byte CRC, reset value
// 1 - Use 2-byte CRC
#define CRCO		0x04

// 0 - Power down the radio, reset value
// 1 - Power up the radio
#define PWR_UP		0x02

// 0 - Radio is a transmitter, reset value
// 1 - Radio is a receiver
#define PRIM_RX		0x01 


/* 
 * EN_AA register bits (Enhanced ShockBurst)
 */

// 0 - Disable Enhanced Shockburst on Rx pipe 5
// 1 - Enable Enhanced Shockburst on Rx pipe 5, reset value
#define ENAA_P5		0x20

// 0 - Disable Enhanced Shockburst on Rx pipe 4
// 1 - Enable Enhanced Shockburst on Rx pipe 4, reset value
#define ENAA_P4		0x10

// 0 - Disable Enhanced Shockburst on Rx pipe 3
// 1 - Enable Enhanced Shockburst on Rx pipe 3, reset value
#define ENAA_P3		0x08

// 0 - Disable Enhanced Shockburst on Rx pipe 2
// 1 - Enable Enhanced Shockburst on Rx pipe 2, reset value
#define ENAA_P2		0x04

// 0 - Disable Enhanced Shockburst on Rx pipe 1
// 1 - Enable Enhanced Shockburst on Rx pipe 1, reset value
#define ENAA_P1		0x02

// 0 - Disable Enhanced Shockburst on Rx pipe 0
// 1 - Enable Enhanced Shockburst on Rx pipe 0, reset value
#define ENAA_P0		0x01


/* 
 * EN_RXADDR Register (Enabled RX Addresses)
 */

// 0 - Disable Rx pipe 5, reset value
// 1 - Enable Rx pipe 5
#define ERX_P5		0x20

// 0 - Disable Rx pipe 4, reset value
// 1 - Enable Rx pipe 4
#define ERX_P4		0x10

// 0 - Disable Rx pipe 3, reset value
// 1 - Enable Rx pipe 3
#define ERX_P3		0x08

// 0 - Disable Rx pipe 2, reset value
// 1 - Enable Rx pipe 2
#define ERX_P2		0x04

// 0 - Disable Rx pipe 1
// 1 - Enable Rx pipe 1, reset value
#define ERX_P1		0x02

// 0 - Disable Rx pipe 0
// 1 - Enable Rx pipe 0, reset value
#define ERX_P0		0x01


/*
 * SETUP_AW Register (Setup of Address Width, common for all data pipes)
 */

// only bit (1:0)
#define	ADDR_WIDTH	0x03
#define ADDR_WIDTH_3_BYTES	0x01
#define ADDR_WIDTH_4_BYTES	0x02
#define ADDR_WIDTH_5_BYTES	0x03


/* 
 * SETUP_RETR (Setup of Automatic Retransmission)
 */

// Auto Retransmit Delay
// 0000 - Wait 250us, reset value
// 0001 - Wait 500us
// 0010 - wait 750us
// ......
// 1111 - wait 4000us
// Delay defined from end of transmission to start of next transmission
#define	ARD		0xF0

// Auto Retransmit Count
// 0000 - Retransmit disabled
// 0001 - Up to 1 retransmit on fail of AA
// ......
// 0011 - Up to 3 retransmit on fail of AA
// ......
// 1111 - Up to 15 retransmit on fail of AA
#define	ARC		0x0E


/* 
 * RF_CH (RF Channel), bit (6:0), reset value 0x02
 */


/* 
 * RF_SETUP Register
 */

// Force PLL lock signal. See http://en.wikipedia.org/wiki/Phase-locked_loop
// 1 - Force PLL lock signal, Only used in test
// 0 - Normal, reset value
#define PLL_LOCK	0x10

// 0 - On-air data rate is 1 Mbps
// 1 - On-air data rate is 2 Mbps, reset value
#define RF_DR		0x08

// Radio Tx Power (bits 2:1)
// 00 -  -18 dBm 
// 01 -  -12 dBm
// 10 -   -6 dBm
// 11 -    0 dBm, reset value
// dBm is decibels relative to 1 mW.
#define RF_PWR		0x06
#define	RF_PWR_HIGH	0x06	// reset value
#define	RF_PWR_MED_H	0x04
#define	RF_PWR_MED_L	0x02
#define RF_PWR_LOW	0x00

// Low-noise amplifier gain
// setup LNA gain, reset value is '1', This should not be clear
#define LNA_HCURR	0x01


/* 
 * STATUS Register
 */

// 0 - Rx data ready interrupt was not triggered
// 1 - Rx data ready interrupt was triggered.  (write 1 to clear after interrupt)
#define RX_DR		0x40

// 0 - Tx data sent interrupt was not triggered
// 1 - Tx data sent interrupt was triggered.  (write 1 to clear after interrupt)
// If Enhanced Shockburst is enabled, the interrupt is triggered once the 
// transmitter receives the ack.
#define TX_DS		0x20

// 0 - Max retries sent interrupt was not triggered
// 1 - Max retries sent interrupt was triggered.  (write 1 to clear after interrupt)
// If the MAX_RT interrupt is triggered, this needs to be cleared before the radio 
// can be used again
#define MAX_RT		0x10

// Number of the data pipe that just received data (bits 3:1)
// 000 to 101 - pipe number 0-5
// 110 not used
// 111 all FIFOs are empty
#define RX_P_NO		0x0E
#define RX_P_NO_P0		0x00
#define RX_P_NO_P1		0x02
#define RX_P_NO_P2		0x04
#define RX_P_NO_P3		0x06
#define RX_P_NO_P4		0x08
#define RX_P_NO_P5		0x0A
#define RX_P_NO_ALL_EMPTY	0x0E

// 0 - There are available locations in the Tx FIFO
// 1 - The Tx FIFO is full.
#define TX_FULL		0x01

#endif
