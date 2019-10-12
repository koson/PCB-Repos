/*
 * Read Analog Input
*/
#define ledPin 13      // select the pin for the LED, on-board LED
#define analogPIN A5
void setup() {
  Serial.begin(115200);
   pinMode(ledPin, OUTPUT);
}
 
void loop() {
  int value = analogRead(analogPIN);                                
  Serial.println(value);        // value in range 0-1023 -> ADC 10 bits  
  int voltage;
  voltage = map(value,0,1023,0,5000);   //map value 0-1023 to 0-5000 (mV)
  Serial.print(voltage);              //mV
  Serial.print(" mV");
  Serial.println();   
  Serial.println();  
  digitalWrite(ledPin, HIGH);
  delay(50);         
  digitalWrite(ledPin, LOW);
}
