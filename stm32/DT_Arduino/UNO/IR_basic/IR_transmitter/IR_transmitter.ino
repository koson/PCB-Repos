#include <IRremote.h>
#include <IRremoteInt.h>
//digital pin 3
IRsend irsend;
void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); 
}
void loop()
{
  delay(1500);
  irsend.sendSony(0x10, 12); // send 0x10, 12 bits
  delay(1500);
  irsend.sendSony(0x10, 12);
}
