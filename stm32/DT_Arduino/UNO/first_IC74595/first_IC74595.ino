/*
n 74HC595 IC in chain configuration. See the image for pinout wiring!.
/OE to GND
SER // Serial In
SRCLK  // Shift register clock
RCLK  // data latch
/SRCLR 
output: 74595 data.
Example input = 10 = 00001010. -> output: QB and QD on, others OFF

*/
#define n 2 //number of 74HC595 used.
#define SERpin 8
#define SRCLKpin 9
#define RCLKpin 10
#define nSRCLRpin 11
uint8_t pinsetup[]={SERpin,SRCLKpin,RCLKpin,nSRCLRpin};
uint8_t data_out[n]={0xFF,0x22}; 
void setup() {
  
  for (int m=0;m<=3;m++)
{
pinMode(pinsetup[m], OUTPUT);
digitalWrite(pinsetup[m],LOW);
}
digitalWrite(nSRCLRpin,HIGH);
}

void loop() {
     write_74HC595(data_out);
}

void write_74HC595(uint8_t* data_Out)
{
  //memory clear
   digitalWrite(11,LOW);
   digitalWrite(11,HIGH);
   for (int p=n-1;p>=0;p--)
   {//p-th 74HC595 
  for (int k=0;k<=7;k++)
  {
    Serial.print((data_Out[p]>>k)&1);
    Serial.println();
     if ((data_Out[p]>>k)&1)
digitalWrite(8,HIGH);
else
digitalWrite(8,LOW);
//clock
digitalWrite(9,HIGH);
digitalWrite(9,LOW);
  }
   }
  //data latching
digitalWrite(10,HIGH);
digitalWrite(10,LOW);
}


