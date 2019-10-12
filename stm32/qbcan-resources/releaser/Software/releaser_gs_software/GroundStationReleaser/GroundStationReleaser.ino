/* 
    qbcan CanSat Releaser Ground Station example.

    This sketch receives data from other qbcan that transmit messages.
    This sketch allows the Ground Station to send Release commands to the qbcan Releaser

    The Release command (and any other user-defined comand) is sent 
    using the Serial Monitor. Use the char value defined in the variable 
    releaseTriggerChar. This value must be identically defined in the qbcan
    Releaser code.
*/

//Include the required libraries
#include <qbcan.h>
#include <Wire.h>
#include <SPI.h>

//Radio Parameters
#define NODEID        1    //unique for each node on same network
#define NETWORKID     100  //the same on all nodes that talk to each other
#define GATEWAYID     5    //Receiving node
#define ENCRYPTKEY    "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!

//Radio object
RFM69 radio;
bool promiscuousModeGS = true; //set to 'true' to sniff all packets on the same network
char * cmdToCansat;
char payloadToCansat[50];

//Releaser options
char releaseTriggerChar = 'R'; // Character used for by the qbcan Releaser
bool reqACK = true; // request acknowledgement from Cansat releaser

//------------------------------------------
//------------------SETUP-------------------
//------------------------------------------

void setup()
{
  //Initialize serial connection for debugging
  Serial.begin(9600);
  Serial.println("REBOOT");

  //Delay to give time to the radio to power up
  delay(1000);

  //Initialize radio
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setHighPower(); //Use the high power capabilities of the RFM69HW
  radio.encrypt(ENCRYPTKEY);
  radio.promiscuous(promiscuousModeGS);
  Serial.println("Listening at 433 Mhz");

}

//------------------------------------------
//------------------LOOP--------------------
//------------------------------------------

uint32_t packetCount = 0;
void loop()
{
  // If Serial communication available (any commands from PC)         
  if (Serial.available() ) {

    // User command acquisition
    cmdToCansat = charParse(1);  
    
    // Send command to cansat
    sprintf(payloadToCansat,"%s",cmdToCansat);
    radio.send(GATEWAYID, payloadToCansat, 50, reqACK); // with ACK request
    
    Serial.print(payloadToCansat);
    Serial.println(" - Send complete");

    delay(50);
  }

  // Listen for transmissions
  if (radio.receiveDone())
  {
    Serial.print("#[");
    Serial.print(++packetCount);
    Serial.print(']');
    Serial.print('[');Serial.print(radio.SENDERID, DEC);Serial.print("] ");
    if (promiscuousModeGS)
    {
      Serial.print("to [");Serial.print(radio.TARGETID, DEC);Serial.print("] ");
    }
    for (byte i = 0; i < radio.DATALEN; i++)
      Serial.print((char)radio.DATA[i]);
    Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]");

    // Acknowledge if requested
    if (radio.ACKRequested())
    {
      byte theNodeID = radio.SENDERID;
      radio.sendACK();
      Serial.print(" - ACK sent.");
    }
    Serial.println();
  }

}

char * charParse (byte length)     // Return parsed byte from serial
{    
  char BUFFER[length + 1];  // Define BUFFER variable
  delay(50);          // Needed for Baud 9600, less delay for faster speed
  
  // Read Serial and store in BUFFER
  for(byte i = 0; i < length; i++)
  {
    BUFFER[i] = Serial.read();  // Read serial char by char
  }
  BUFFER[length] = 0;     // Set last value as 0

  //return atoi(BUFFER);    // Convert string to integer and return
  return BUFFER;
}

