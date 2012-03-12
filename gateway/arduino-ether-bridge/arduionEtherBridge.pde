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

#include <SPI.h>

#include <etherShield.h>
#include <nrf24l01.h>

#include "ihome.h"

static uint8_t mymac[6] = {0x54,0x55,0x58,0x10,0x00,0x24}; 
static uint8_t myip[4] = {192,168,1,15};

static char baseurl[] = "http://192.168.1.15/";
static uint16_t mywwwport = 80; // listen port for tcp/www (max range 1-254)

#define BUF_SIZE        500
#define STR_BUF_SIZE	8

static uint8_t buf[BUF_SIZE+1];
static char devidbuf[STR_BUF_SIZE+1];
static char cmdbuf[STR_BUF_SIZE+1];

int16_t devid, cmdid;

// prepare the webpage by writing the data to the tcp send buffer
uint16_t print_web (uint8_t *buf);
int8_t analyse_cmd (uint8_t *str);

#define ID_LEN    5
byte sys_id[ID_LEN] = {0xBF, 0xA3, 0x21, 0x23, 0x01};
byte tx_buf[32], rx_buf[32];

NRF24L01 radio(7, 6, 5);
EtherShield eth;

void setup()
{
        SPI.begin();
        eth.begin();
        
  	radio.trans_init();
	radio.tx_config(sys_id, ID_LEN);
	radio.rx_config(0, sys_id, ID_LEN, 32);

        Serial.begin(9600);
}

void loop()
{
	uint16_t plen, dat_p;
	int8_t cmd;
	byte on_off = 1;

	plen = eth.pkt_rx (buf, BUF_SIZE);

	/*plen will ne unequal to zero if there is a valid packet (without crc error) */
	if(plen != 0)
	{
		// arp is broadcast if unknown but a host may also verify the
		// mac address by sending it to a unicast address.
		if(eth.is_arp_and_my_ip (buf,plen))
		{
			eth.make_arp_answer (buf);
			return;
		}

		// check if ip packets are for us
		if(eth.is_ip_and_my_ip (buf, plen) == 0)
			return;

		if(buf[IP_PROTO_P] == IP_PROTO_ICMP_V &&
			buf[ICMP_TYPE_P] == ICMP_TYPE_ECHOREQUEST_V)
		{
			eth.make_echo_reply (buf, plen);
			return;
		}

		// tcp port www start, compare only the lower byte
		if (buf[IP_PROTO_P] == IP_PROTO_TCP_V && buf[TCP_DST_PORT_H_P] == 0
			&& buf[TCP_DST_PORT_L_P] == mywwwport)
		{
			if (buf[TCP_FLAGS_P] & TCP_FLAGS_SYN_V)
			{
				eth.make_tcp_synack (buf); // make_tcp_synack_from_syn does already send the syn,ack
				return;     
			}

			if (buf[TCP_FLAGS_P] & TCP_FLAGS_ACK_V)
			{
				eth.len_info_init(buf); // init some data structures

                                dat_p = eth.get_tcp_data_ptr();

				if (dat_p == 0)
				{ 
					// we can possibly have no data, just ack:
					if (buf[TCP_FLAGS_P] & TCP_FLAGS_FIN_V)
						eth.make_tcp_ack_nodata (buf);
					return;
				}


				if (strncmp("GET ", (char *)&(buf[dat_p]),4) != 0)
				{
					// head, post and other methods for possible status codes see:
					// http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
					plen=eth.fill_tcp_p (buf, 0, PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>200 OK</h1>"));
					goto SENDTCP;
				}

				if (strncmp("/ ",(char *)&(buf[dat_p+4]),2)==0)
				{
					plen=print_web (buf);
					goto SENDTCP;
				}

				if(process_cmd((char *)&(buf[dat_p+5])) && devid != 0 && cmdid != 0)
				{
					//call nrf24l to send the package
					//ship the tx_buf;
                                        memset(tx_buf, 0, 32);
					tx_buf[0] = T_CMD;
					tx_buf[1] = (devid >> 8) & 0xFF;
					tx_buf[2] = devid & 0xFF;
					tx_buf[3] = CENTRAL_GW;
					tx_buf[4] = 0xFF;
					tx_buf[5] = 0x01;
					tx_buf[6] = cmdid;

                                        Serial.print("tx_buf[1] = ");
                                        Serial.println(tx_buf[1], HEX);
                                        Serial.print("tx_buf[2] = ");
                                        Serial.println(tx_buf[2], HEX);
                                        Serial.print("cmd = ");
                                        Serial.println(cmdid, HEX);

					radio.tx(tx_buf, 32);
                                        byte _tmp = 0xFF;
					if((_tmp = radio.check_tx_flag()) == 1)
					{
						//waitting for switch success radio message
						Serial.println("TX OK!");//print sucess web page
					}
					else
                                        {
						Serial.println("TX faild");//re-tx
                                                Serial.println(_tmp, HEX);
                                        }

				}
				else
                                {
					Serial.println("Can not get the correct devid and cmdid");//print_web can not get correct devid and cmd
                                }
				plen=print_web (buf);

		SENDTCP:	eth.make_tcp_ack_nodata(buf); // send ack for http get
				eth.make_tcp_ack(buf,plen); // send data       
			}
		}
	}

}

// The returned value is stored in the global var strbuf
uint8_t find_key_val(char *str, char *key, char *op)
{
        uint8_t found=0;
        uint8_t i=0;
        char *kp;
        kp=key;
        while(*str &&  *str!=' ' && found==0){
                if (*str == *kp){
                        kp++;
                        if (*kp == '\0'){
                                str++;
                                kp=key;
                                if (*str == '='){
                                        found=1;
                                }
                        }
                }else{
                        kp=key;
                }
                str++;
        }
        if (found==1){
                // copy the value to a buffer and terminate it with '\0'
                while(*str &&  *str!=' ' && *str!='&' && i<STR_BUF_SIZE){
                        op[i]=*str;
                        i++;
                        str++;
                }
                op[i]='\0';
        }
        return(found);
}

uint16_t str2short(char *abuf)
{
	char *p = abuf;
	uint16_t tt = 0;
	uint8_t i;

        //Serial.println(p);
	//for(i=0 ; i<4; i++)
	while(*p != '\0')
	{
		if(*p >= 0x30 && *p <= 0x39)	
		{
			tt <<= 4;
			tt |= (*p - 0x30);
                        //Serial.println(tt, HEX);
		}
		if(*p >= 0x61 && *p <= 0x66)
		{
			tt <<= 4;
			tt |= (*p - 0x57);
                        //Serial.println(tt, HEX);
		}
		if(*p >= 0x41 && *p <= 0x46)
		{
			tt <<=4;
			tt |= (*p - 0x37);
                        //Serial.println(tt, HEX);
		}
                p++;
	}
        return tt;
}

/*
 * 0 ~ 9:  30 ~ 39
 * a ~ f:  61 ~ 66
 * A ~ F:  41 ~ 46
*/
uint8_t process_cmd(char *str)
{
        if (find_key_val(str,"devid", devidbuf)){
		devid = str2short(devidbuf);
                Serial.println("find devid");
        }
        else
                return 0;
        
        if (find_key_val(str,"cmd", cmdbuf)){
		cmdid = str2short(cmdbuf);
                Serial.println("find cmdid");
        }
	else
		return 0;

                                        Serial.print("devid=");
                                        Serial.print(devid, HEX);
                                        Serial.print(" cmdid=");
                                        Serial.print(cmdid, HEX);
                                        Serial.println(" ");

	return 1;
}


uint16_t print_web(uint8_t *buf)
{
        uint16_t plen;
        
        plen=eth.fill_tcp_p (buf, 0, PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"));
        plen=eth.fill_tcp_p (buf, plen, PSTR("<center><p><h1>Welcome to Smart Home Gateway V1.0  </h1></p> "));
         
        plen=eth.fill_tcp_p (buf, plen, PSTR("<hr><br><form METHOD=get action=\""));

        plen=eth.fill_tcp (buf, plen, baseurl);

        plen=eth.fill_tcp_p (buf, plen, PSTR("\">"));
        
	plen=eth.fill_tcp_p (buf, plen, PSTR("Device ID: <input type=text name=devid maxlength=4>"));
	plen=eth.fill_tcp_p (buf, plen, PSTR("Commands: <input type=text name=cmd maxlength=4>"));
	plen=eth.fill_tcp_p (buf, plen, PSTR("<input type=submit value=\"Submit\"></form>"));
        plen=eth.fill_tcp_p (buf, plen, PSTR("</center><hr> <p> V1.0 <a href=\"http://www.jackslab.org\">www.jackslab.org<a>"));
  
        return(plen);
}
