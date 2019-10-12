/* 
    qbcan ground station example.

    This sketch receives data from other qbcan that transmit messages and also prints the ground station's tempearture and pressure readings.
*/

//Include the required libraries
#include <qbcan.h>
#include <Wire.h>
#include <SPI.h>

//Pressure sensor object
BMP180 bmp;

//Radio Parameters
#define NODEID        1    //unique for each node on same network
#define NETWORKID     100  //the same on all nodes that talk to each other
#define ENCRYPTKEY    "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!

//Radio object
RFM69 radio;
bool promiscuousMode = false; //set to 'true' to sniff all packets on the same network

void setup()
{
  //Initialize serial connection for debugging
  Serial.begin(9600);
  Serial.println("REBOOT");

 // Initialize pressure sensor.
  if (bmp.begin())
    Serial.println("BMP180 init success");
  else
  {
    //In case of error let user know of the problem
    Serial.println("BMP180 init fail (disconnected?)\n\n");
    while(1); // Pause forever.
  }


  //Delay to give time to the radio to power up
  delay(1000);

  //Initialize radio
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setHighPower(); //Use the high power capabilities of the RFM69HW
  radio.encrypt(ENCRYPTKEY);
  radio.promiscuous(promiscuousMode);
  Serial.println("Listening at 433 Mhz");

}


byte ackCount=0;
uint32_t packetCount = 0;
void loop()
{
  double T,P;
   // Get a new pressure reading:
  bmp.getData(T,P);
    //Display data
      Serial.println("Ground station readings: ");
  Serial.print("Absolute pressure: ");
  Serial.print(P,2);
  Serial.println(" mb.");
  Serial.print("Temperature: ");
  Serial.print(T,2);
  Serial.println(" deg C.");

  if (radio.receiveDone())
  {
    Serial.print("#[");
    Serial.print(++packetCount);
    Serial.print(']');
    Serial.print('[');Serial.print(radio.SENDERID, DEC);Serial.print("] ");
    if (promiscuousMode)
    {
      Serial.print("to [");Serial.print(radio.TARGETID, DEC);Serial.print("] ");
    }
    for (byte i = 0; i < radio.DATALEN; i++)
      Serial.print((char)radio.DATA[i]);
    Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]");
    
    if (radio.ACKRequested())
    {
      byte theNodeID = radio.SENDERID;
      radio.sendACK();
      Serial.print(" - ACK sent.");

      // When a node requests an ACK, respond to the ACK
      // and also send a packet requesting an ACK (every 3rd one only)
      // This way both TX/RX NODE functions are tested on 1 end at the GATEWAY
      if (ackCount++%3==0)
      {
        Serial.print(" Pinging node ");
        Serial.print(theNodeID);
        Serial.print(" - ACK...");
        delay(3); //need this when sending right after reception .. ?
        if (radio.sendWithRetry(theNodeID, "ACK TEST", 8, 0))  // 0 = only 1 attempt, no retries
          Serial.print("ok!");
        else Serial.print("nothing");
      }
    }
    Serial.println();
  }
  delay(500);
}



