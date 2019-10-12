/*
 * test all led in LED8x8 matrix
 *  0-7 pins to all LED8x8 8 columns ( cathode )
 * 8-15 to ... row ( anode )
*/
#define leddelay 250 //led delay time in ms
int ledPin[16]={1,2,3,4,5,6,7,
8,9,10,11,12,13,14,15,16};
int c=0;
int  r=0;
void setup() {
//set all used pins to HIGH
for (int m=0;m<=16;m++)
{
pinMode(ledPin[m], OUTPUT);
digitalWrite(ledPin[m],HIGH);
}
}

void loop() {
if (r==8)
r=0;
  if (c==8)
  {
c=0;
r++; //done loop 1 row, go to next row
  }
onled(r,c);
delay(leddelay);
c++;
}
void onled(int r,int c) {
//input indices of led[r,c] to ON others LED OFF
  for (int k=0;k<=7;k++)
  {
  if (k==r)
  digitalWrite(ledPin[k+8],HIGH);
  else
  digitalWrite(ledPin[k+8],LOW);
  }
  
  for (int k=0;k<=7;k++)
{
  if (k==c)
  digitalWrite(ledPin[k],LOW);
  else
  digitalWrite(ledPin[k],HIGH);
}
}

