/* 
    qbcan CanSat Releaser example.

    This sketch reads temperature and pressure data and sends it to the Ground station
    It receives the release command from the Ground Station and releases the cansat.

    The Release command (and any other user-defined comand) is sent 
    using the PC Serial Monitor which is connected to the Ground Station qbcan. 
    The value defined in the variable releaseTriggerChar must be identically 
    defined in the Ground Station Releaser qbcan code.
*/

//Include the required libraries
#include <qbcan.h>
#include <Wire.h>
#include <SPI.h>

//Pressure sensor object
BMP180 bmp;

//Radio Parameters
#define NODEID        5    //unique for each node on same network
#define NETWORKID     100  //the same on all nodes that talk to each other
#define GATEWAYID     1    //Receiving node
#define ENCRYPTKEY    "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!

//Radio object
uint8_t payloadLEN = 50; // This value must be the same as in the line below 'payload[50]'
char payload[50];
RFM69 radio;
bool promiscuousMode = false; //set to 'true' to sniff all packets on the same network

//Releaser options
int motorTime = 150;  // number of miliseconds the motor is ON
int gatePin = 6;      // PIN connected to the transistor gate that drives the motor.
char releaseTriggerChar = 'R'; // Character used for by the qbcan Releaser

int RXLED = 17;  // The RX LED has a defined Arduino pin

//------------------------------------------
//------------------SETUP-------------------
//------------------------------------------

void setup()
{
  //Initialize serial connection for debugging
  Serial.begin(9600);
  Serial.println("REBOOT");

  // Initialize pressure sensor.
  bool flagBMP = false; // Flag to know if well initilialised - LED use
  if (bmp.begin())
  {
    Serial.println("BMP180 init success");
    flagBMP = true; //used by ledOK function
  } else {
    //In case of error let user know of the problem
    Serial.println("BMP180 init fail (disconnected?)\n\n");
    //while(1); // Pause forever. -- COMMENTED
  }

  //Initialize radio
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setHighPower(); //To use the high power capabilities of the RFM69HW
  radio.encrypt(ENCRYPTKEY);
  radio.promiscuous(promiscuousMode);
  Serial.println("Transmitting at 433 Mhz");
  
  // TX LED is set as an output behind the scenes
  pinMode(RXLED, OUTPUT);  // Set RX LED as an output
 
  //Setup releaser option
  pinMode(gatePin, OUTPUT);      // sets the digital pin as output

  // Switch ON and OFF LEDs
  ledOK(flagBMP);

}

//------------------------------------------
//------------------LOOP--------------------
//------------------------------------------

uint32_t packetCount = 0;
uint8_t countRelease = 0;

void loop()
{
  double T,P;
  
  // Get a new pressure reading:
  bmp.getData(T,P);

  if (radio.receiveDone())
  {
    if (radio.DATALEN >= 1){
      if (radio.DATA[0] == releaseTriggerChar){
        
        // If release cmd is received, call release function
        releaseCansat(motorTime, countRelease);
        ledOK(true); // LEDs

        Serial.print("#[");
        Serial.print(++packetCount);
        Serial.print(']');
        Serial.print('[');Serial.print(radio.SENDERID, DEC);Serial.print("] ");
        Serial.print("[Release CMD # ");Serial.print(countRelease);Serial.print("] ");
        Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]");
      }
    } else {
      // ADD FUNCTIONALITIES FOR DIFFERENT RECEIVED COMMANDS - HERE
      for (byte i = 0; i < radio.DATALEN; i++)
      Serial.print((char)radio.DATA[i]);
    }

    // Send ACK and DATA if requested by the Ground Station
    if (radio.ACKRequested())
    {
      delay(500);
      byte theNodeID = radio.SENDERID;
      radio.sendACK();
      Serial.print(" - ACK sent.");

      sprintf(payload,"Released! [#%d] T: %d C, P: %d mb.",(int)countRelease,(int)T,(int)P);
      radio.send(GATEWAYID, payload, payloadLEN);
      Serial.println(payload);Serial.print(" - Send complete");
    }
    Serial.println();
  }
  
}

void releaseCansat(const int motorTimeON, uint8_t &counterRelease)
{
  // Motor ON
  digitalWrite(gatePin, HIGH);  // turn the Motor on (HIGH is the voltage level)
  // LED ON
  digitalWrite(RXLED, LOW);   // set the LED on
  TXLED1; //TX LED is not tied to a normally controlled pin
  delay(motorTimeON);              // wait for a second

  // Motor OFF
  digitalWrite(gatePin, LOW);    // turn the Motor off by making the voltage LOW
  // LED OFF
  digitalWrite(RXLED, HIGH);    // set the LED off
  TXLED0;
  delay(motorTimeON);              // wait for a second

  counterRelease++;
}

void ledOK(bool flagOK)
{
  int delayT = 250; // miliseconds

  if (flagOK)
  {     // If there is no problem, blink 3 times.
    
    digitalWrite(RXLED, LOW);   // set the LED on
    TXLED0; //TX LED is not tied to a normally controlled pin
    delay(delayT);              // wait for a second
    digitalWrite(RXLED, HIGH);    // set the LED off
    TXLED1;
    delay(delayT);              // wait for a second
    
    digitalWrite(RXLED, LOW);   // set the LED on
    TXLED0; //TX LED is not tied to a normally controlled pin
    delay(delayT);              // wait for a second
    digitalWrite(RXLED, HIGH);    // set the LED off
    TXLED1;
    delay(delayT);              // wait for a second
    
    digitalWrite(RXLED, LOW);   // set the LED on
    TXLED0; //TX LED is not tied to a normally controlled pin
    delay(delayT);              // wait for a second
    digitalWrite(RXLED, HIGH);    // set the LED off
    TXLED1;
    delay(delayT);              // wait for a second
    TXLED0;
    
  } else {    // If there's a ploblem, blink once
    
    digitalWrite(RXLED, LOW);   // set the LED on
    TXLED0; //TX LED is not tied to a normally controlled pin
    delay(delayT);              // wait for a second
    digitalWrite(RXLED, HIGH);    // set the LED off
    TXLED1;
    delay(delayT);              // wait for a second
    TXLED0;

  }
}

