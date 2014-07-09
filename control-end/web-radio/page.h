#include <avr/pgmspace.h>

char resp_200[] PROGMEM = {
"HTTP/1.0 200 OK\r\n"
"Server: SHG V1.0\r\n"
"Content-Type: text/html\r\n\r\n"
};

char resp_400[] PROGMEM = {
  "HTTP/1.1 404 Not Found\nServer: SHG V1.0\nContent-Type: text/html\r\n\n<html><h1>Page Not Found</h1></html>"
};

// index page
char index_title[] PROGMEM = {
"<html><title>Smart Home of GAGA and MT</title>"
};

PROGMEM char index_header1[] = {
"<style>\n\r"
"body { font-family: Verdana, Arial; font-size: 9pt; color: #666666}\n\r"
"input { border: 1px solid #0066cc; }\n\r"
".button { background: #eeeeee;}\n\r"
"tr { background-color: #ffffff; height: 30px;}\n\r" 
};

PROGMEM char index_header2[] = {
"td { font-family: Verdana, Arial; font-size: 9pt; text-align: center; vertical-align: middle; height: 45px;}\n\r"
"hr { height:1px; border:none; border-top:1px dashed #0066cc}\n\r"
"</style>\n\r "
};

PROGMEM char index_body0[] = {
"<body><center><p><h1>Smart Home System of MT and GAGA V1.0 </h1></p><hr><br>\n\r"
"<form METHOD=get action= >\n\r"
"Device ID: <input type=text name=devid maxlength=4>\n\r"
"CMD ID: <input type=text name=cmdid maxlength=4>\n\r"
};
PROGMEM char index_body1[] = {
"<input type=submit value=Submit class=button>\n\r"
"</form><br></center><hr><br>\n\r"
"<table width=300 cellspacing=1 bgcolor=#0066cc border=0 align=center>\n\r"
};
PROGMEM char index_body2[] = {
"<tr><td width=50%>BedroomLight</td><td width=50%><a href=/?devid=2003> Switch </a></td></tr>\n\r"
"<tr><td>BathroomLight</td><td><a href=/?devid=2005> Switch </a></td></tr>\n\r"
};
PROGMEM char index_body3[] = {
"<tr><td>LivingRoomLight</td><td><a href=/?devid=2001> Switch </a></td></tr>\n\r"
"<tr><td>DiningRoomLight</td><td><a href=/?devid=2004> Switch </a></td></tr>\n\r"
};
PROGMEM char index_body4[] = {
"<tr><td>KitchenLight</td><td><a href=/?devid=2002> Switch </a></td></tr>\n\r"
"<tr><td>Control ALL Light</td><td><a href=/?devid=2000> Turn Off all </a></td></tr>\n\r"
};
PROGMEM char index_body5[] = {
"<tr><td>Airer Light</td><td><a href=/?devid=3001> Switch </a></td></tr>\n\r"
"<tr><td>Airer Up</td><td><a href=/?devid=3002> Up </a></td></tr>\n\r"
};
PROGMEM char index_body6[] = {
"<tr><td>Airer Stop</td><td><a href=/?devid=3003> Stop </a></td></tr>\n\r"
"<tr><td>Airer Down</td><td><a href=/?devid=3004> Down </a></td></tr>\n\r"
};

PROGMEM char index_footer[] = {
"</table><br><hr> <p> Powered By <a href=http://jackslab.org> Jack's Lab</a></p></body></html>"
};

#define	PAGE_SEC_NUM	11

PGM_P index_page[] PROGMEM = {index_title, index_header1, index_header2, index_body0, index_body1, index_body2, index_body3, index_body4, index_body5, index_body6, index_footer};


