#include<Servo.h>
 
Servo ESC; 
 
int val; //Đọc biến trở
int vel; //Độ dài xung gửi cho ESc
 
void setup()
{
  ESC.attach(9);
}
 
void loop()
{
  val=analogRead(A5);
  vel=map(val,0,1023,1000,2000);
  ESC.writeMicroseconds(vel);
}
