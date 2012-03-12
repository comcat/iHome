/*
  EHTERSHIELD_H library for Arduino etherShield
  Copyright (c) 2012 Jack Tan (www.jackslab.org).  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

extern "C" {
	#include "enc28j60.h"
	#include "ip_arp_udp_tcp.h"
 
}

#include "EtherShield.h"

static uint8_t mymac[6] = {0x54,0x55,0x58,0x10,0x00,0x24}; 
static uint8_t myip[4] = {192,168,1,15};

#define	LED_PIN		4

//constructor
EtherShield::EtherShield()
{
	memcpy(mac_addr, mymac, 6);
	memcpy(ip_addr, myip, 4);
}

EtherShield::EtherShield(uint8_t *mac, uint8_t *ip)
{
	memcpy(mac_addr, mac, 6);
	memcpy(ip_addr, ip, 4);
}

void EtherShield::begin()
{
	enc28j60Init(mac_addr);
//	enc28j60clkout(2);
//	delay(10);

/*
	// LEDA=greed LEDB=yellow
	// 0x880 is PHLCON LEDB=on, LEDA=on
	// enc28j60PhyWrite(PHLCON,0b0000 1000 1000 00 00);
	enc28j60PhyWrite(PHLCON, 0x880);
	delay(500);

	// 0x990 is PHLCON LEDB=off, LEDA=off
	// enc28j60PhyWrite(PHLCON,0b0000 1001 1001 00 00);
	enc28j60PhyWrite(PHLCON, 0x990);
	delay(500);

	// 0x880 is PHLCON LEDB=on, LEDA=on
	// enc28j60PhyWrite(PHLCON,0b0000 1000 1000 00 00);
	enc28j60PhyWrite(PHLCON, 0x880);
	delay(500);

	// 0x990 is PHLCON LEDB=off, LEDA=off
	// enc28j60PhyWrite(PHLCON,0b0000 1001 1001 00 00);
	enc28j60PhyWrite(PHLCON, 0x990);
	delay(500);

	// 0x476 is PHLCON LEDA=links status, LEDB=receive/transmit
	// enc28j60PhyWrite(PHLCON,0b0000 0100 0111 01 10);
	enc28j60PhyWrite(PHLCON, 0x476);
	delay(100);
	*/

	// 0x476 is PHLCON LEDA=links status, LEDB=receive/transmit
	// enc28j60PhyWrite(PHLCON,0b0000 0100 0111 01 10);
	enc28j60PhyWrite(PHLCON,0x476);
	delay(12);
        
	//init the ethernet/ip layer:
	init_ip_arp_udp_tcp (mac_addr, ip_addr, 80);

 	pinMode(LED_PIN, OUTPUT); 
 	digitalWrite(LED_PIN, LOW);  // switch on LED
}

uint16_t EtherShield::pkt_rx (uint8_t* packet, uint16_t len)
{
	return enc28j60PacketReceive(len, packet);
}

uint16_t EtherShield::fill_tcp_p (uint8_t *buf,uint16_t pos, const prog_char *progmem_s)
{
	return fill_tcp_data_p(buf, pos, progmem_s);
}

uint16_t EtherShield::fill_tcp (uint8_t *buf,uint16_t pos, const char *s)
{
	return fill_tcp_data(buf, pos, s);
}


uint8_t EtherShield::is_arp_and_my_ip (uint8_t *buf,uint16_t len)
{
	return eth_type_is_arp_and_my_ip(buf,len);
}

void EtherShield::make_arp_answer (uint8_t *buf)
{
	make_arp_answer_from_request(buf);
}

uint8_t EtherShield::is_ip_and_my_ip (uint8_t *buf,uint16_t len)
{
	return eth_type_is_ip_and_my_ip(buf, len);
}


void EtherShield::make_echo_reply (uint8_t *buf,uint16_t len)
{
	make_echo_reply_from_request(buf,len);
}

void EtherShield::make_tcp_synack (uint8_t *buf)
{
	make_tcp_synack_from_syn(buf);
}	

void EtherShield::len_info_init(uint8_t *buf)
{
	init_len_info(buf);
}

uint16_t EtherShield::get_tcp_data_ptr(void)
{
	return get_tcp_data_pointer();
}

void EtherShield::make_tcp_ack_nodata(uint8_t *buf)
{
	make_tcp_ack_from_any(buf);
}

void EtherShield::make_tcp_ack(uint8_t *buf,uint16_t dlen)
{
	make_tcp_ack_with_data(buf,dlen);
}

void EtherShield::make_arp_req(uint8_t *buf, uint8_t *server_ip)
{
	make_arp_request(buf, server_ip);
}

uint8_t EtherShield::is_myreply_arp ( uint8_t *buf )
{
	return arp_packet_is_myreply_arp (buf);
}

void EtherShield::tcp_client_send_pkt (uint8_t *buf, uint16_t dest_port,
		uint16_t src_port, uint8_t flags, uint8_t max_segment_size, 
		uint8_t clear_seqck, uint16_t next_ack_num, uint16_t dlength,
		uint8_t *dest_mac, uint8_t *dest_ip)
{
	tcp_client_send_packet(buf, dest_port, src_port, flags, max_segment_size,
		clear_seqck, next_ack_num, dlength,dest_mac,dest_ip);
}

uint16_t EtherShield::tcp_get_dlen (uint8_t *buf)
{
	return tcp_get_dlength(buf);
}
