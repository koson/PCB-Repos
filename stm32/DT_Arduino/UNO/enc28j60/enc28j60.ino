#include <enc28j60.h>
#include <EtherCard.h>
#include <net.h>

// This is a demo of the RBBB running as webserver with the Ether Card
// 2010-05-28 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php
/* library:https://github.com/jcw/ethercard
 * credit: https://www.instructables.com/id/Add-Ethernet-to-any-Arduino-project-for-less-than-/
Wire up as following (ENC = module side):
- ENC SO -> Arduino pin 12
- ENC SI -> Arduino pin 11
- ENC SCK -> Arduino pin 13
- ENC CS -> Arduino pin 10
- ENC VCC -> Arduino 3V3 pin
- ENC GND -> Arduino Gnd pin
*/
#include <EtherCard.h>

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
static byte myip[] = { 192,168,1,115 };

byte Ethernet::buffer[500];
BufferFiller bfill;

void setup () {
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
    Serial.println( "Failed to access Ethernet controller");
  ether.staticSetup(myip);
}

static word homePage() {
  long t = millis() / 1000;
  word h = t / 3600;
  byte m = (t / 60) % 60;
  byte s = t % 60;
  bfill = ether.tcpOffset();
  bfill.emit_p(PSTR(
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Pragma: no-cache\r\n"
    "\r\n"
    "<meta http-equiv='refresh' content='1'/>"
    "<title>RBBB server</title>"
    "<h1>$D$D:$D$D:$D$D</h1>"),
      h/10, h%10, m/10, m%10, s/10, s%10);
  return bfill.position();
}

void loop () {
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
  
  if (pos)  // check if valid tcp data is received
    ether.httpServerReply(homePage()); // send web page data
}
