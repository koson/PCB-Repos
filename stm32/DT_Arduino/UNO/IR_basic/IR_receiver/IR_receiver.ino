/*
Read infrared signals.
credit: http://arduino.vn/bai-viet/288-infrared-remote-control-dieu-khien-bang-hong-ngoai-voi-arduino
library: https://github.com/z3t0/Arduino-IRremote
pin (1) (2) (3) respectively are gnd vcc out
*/

#include <IRremote.h>
#include <IRremoteInt.h>
//IRsend irsend;
const int receiverPin = 8; 
IRrecv irrecv(receiverPin);
decode_results results;
void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); 
}
void loop()
{
 // irsend.sendSony(0x10, 12);
  if (irrecv.decode(&results)) // if successfully receive the signal
  {
    Serial.println(results.value, HEX); // print to Serial Monitor
    delay(200);
    irrecv.resume(); // continue ready for coming signals 
  }
}
