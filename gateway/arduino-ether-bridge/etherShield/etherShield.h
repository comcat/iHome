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

#ifndef ETHERSHIELD_H
#define ETHERSHIELD_H

#include <inttypes.h>
#include <avr/pgmspace.h>
#include <WProgram.h>

#include "enc28j60.h"
#include "ip_arp_udp_tcp.h"
#include "net.h"


class EtherShield
{
	public:
		EtherShield();
		EtherShield(uint8_t *mac, uint8_t *ip);

		void begin();

		uint16_t pkt_rx(uint8_t* packet, uint16_t len);
		uint16_t fill_tcp_p (uint8_t *buf,uint16_t pos, const prog_char *progmem_s);
		uint16_t fill_tcp (uint8_t *buf,uint16_t pos, const char *s);

		uint8_t is_arp_and_my_ip (uint8_t *buf,uint16_t len);
		uint8_t is_ip_and_my_ip (uint8_t *buf,uint16_t len);
		uint8_t is_myreply_arp ( uint8_t *buf );

		void len_info_init(uint8_t *buf);
		uint16_t get_tcp_data_ptr(void);

		void make_arp_answer (uint8_t *buf);
		void make_echo_reply (uint8_t *buf,uint16_t len);
		void make_tcp_synack(uint8_t *buf);
		void make_tcp_ack_nodata(uint8_t *buf);
		void make_tcp_ack(uint8_t *buf,uint16_t dlen);
		void make_arp_req(uint8_t *buf, uint8_t *server_ip);

		void tcp_client_send_pkt (uint8_t *buf, uint16_t dest_port,
				uint16_t src_port, uint8_t flags, uint8_t max_segment_size, 
				uint8_t clear_seqck, uint16_t next_ack_num, uint16_t dlength,
				uint8_t *dest_mac, uint8_t *dest_ip);

		uint16_t tcp_get_dlen (uint8_t *buf);

	private:
		uint8_t mac_addr[6];
		uint8_t ip_addr[4];
};	
		
#endif

