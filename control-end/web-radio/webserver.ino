
/*
  Web Server
 
 A simple web server that shows the value of the analog input pins.
 using an iBoard. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13

 Created by
 Jack Tan <jiankemeng@gmail.com>
 
 */

#include <SPI.h>
#include <Ethernet.h>
#include <RadioTX.h>
#include <TextFinder.h>

#include "ihome.h"
#include "page.h"

static byte mac[] = { 0xDE, 0xAD, 0xCE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 251);

EthernetServer server(80);

void setup()
{
	Serial.begin(9600);

	// wait for serial port to connect. Needed for Leonardo only
	while (!Serial) {
		;
	}

	// start the Ethernet connection and the server:
	Ethernet.begin(mac, ip);
	server.begin();
	Serial.print("server is at ");
	Serial.println(Ethernet.localIP());
}

void loop()
{
	// listen for incoming clients
	EthernetClient client = server.available();

	if (client) {

		TextFinder finder(client);

		Serial.println("new client");

		// an http request ends with a blank line
		boolean currentLineIsBlank = true;

		while (client.connected()) {

			if (client.available()) {
				char c = client.read();
				Serial.write(c);

				// if you've gotten to the end of the line (received a newline
				// character) and the line is blank, the http request has ended,
				// so you can send a reply
			//	if (c == '\n' && currentLineIsBlank) {


					if ( finder.findUntil("devid=", "\n\r") ) {
							uint16_t devid = finder.getValue();

							uint16_t cmdid = 0;
							if(finder.findUntil("cmdid=", "\n\r"))
								cmdid = finder.getValue();

							Serial.print("\n\r");
							Serial.print("devid=");
							Serial.print(devid);
							Serial.print(" cmdid=");
							Serial.print(cmdid);
							Serial.println(" ");

							handle_cmd(devid, cmdid);
					}
			
					// send a standard http response header
					client.println("HTTP/1.1 200 OK");
					client.println("Content-Type: text/html");
					client.println("Connnection: close");
					client.println();
					home_page(client);
					break;
			//	}
				if (c == '\n') {
					// you're starting a new line
					currentLineIsBlank = true;
				}
				else if (c != '\r') {
					// you've gotten a character on the current line
					currentLineIsBlank = false;
				}
			}
		}
		// give the web browser time to receive the data
		delay(1);
		// close the connection:
		client.stop();
		Serial.println("client disonnected");
	}
}

#define BUFFER_SIZE	64	

void page_print(EthernetClient &client, PGM_P realword)
{
	uint8_t total = 0; 
	uint8_t start = 0;
	uint8_t len = strlen_P(realword);
	char buffer[BUFFER_SIZE];

	memset(buffer, 0, BUFFER_SIZE);

	while(total <= len)
	{
		strncpy_P(buffer, realword+start, BUFFER_SIZE-1);
		client.print(buffer);

		total = start + BUFFER_SIZE - 1;
		start += BUFFER_SIZE - 1;
	}
}

void home_page(EthernetClient & client)
{
	// 5 section
	for(uint8_t i = 0; i < PAGE_SEC_NUM; i++)
	{
		page_print(client, (PGM_P)pgm_read_word(&(index_page[i])));
	}
}

void handle_cmd(uint16_t &devid, uint16_t &cmdid)
{
	RadioTX radio(A5);
	RadioTX airer(A0,200);
        uint16_t tmp;

	switch (devid)
	{
		case 2003:
			radio.issue(0x9f2f11);
			Serial.println("Toggle bedroom light");
			break;
		case 2005:
                        tmp = radio.get_ick();
                        radio.set_ick(300);
			radio.issue(0xcf7cf2);
                        radio.set_ick(tmp);
			Serial.println("Toggle bathroom light");
			break;
		case 2001:
			radio.issue(0x9f2f18);
			Serial.println("Toggle living room light");
			break;
		case 2004:
			radio.issue(0x9f2f14);
			Serial.println("Toggle dining room light");
			break;
		case 2002:
			Serial.println("Toggle kitchroom light");
			break;
		case 2000:
			radio.issue(0x9f2f12);
			Serial.println("Ture all light off");
			break;
		case 3001:
			airer.issue_airer(0xA31080FD,0x020053E2);
			airer.issue_airer(0xA31080FD,0x020053E2);
			airer.issue_airer(0xA31080FD,0x020053E2);
			airer.issue_airer(0xA31080FD,0x020053E2);
			airer.issue_airer(0xA31080FD,0x020053E2);
			airer.issue_airer(0xA31080FD,0x020053E2);
			airer.issue_airer(0xA31080FD,0x020053E2);
			Serial.println("Toggle Airer Light");
			break;
		case 3002:
			airer.issue_airer(0xA31080FD,0x02000B9A);
			airer.issue_airer(0xA31080FD,0x02000B9A);
			airer.issue_airer(0xA31080FD,0x02000B9A);
			airer.issue_airer(0xA31080FD,0x02000B9A);
			airer.issue_airer(0xA31080FD,0x02000B9A);
			Serial.println("Airer Up");
			break;
		case 3003:
			airer.issue_airer(0xA31080FD,0x02002DBC);
			airer.issue_airer(0xA31080FD,0x02002DBC);
			Serial.println("Airer Stop");
			break;
		case 3004:
			airer.issue_airer(0xA31080FD,0x02004BDA);
			airer.issue_airer(0xA31080FD,0x02004BDA);
			Serial.println("Airer Down");
			break;
	}
}
