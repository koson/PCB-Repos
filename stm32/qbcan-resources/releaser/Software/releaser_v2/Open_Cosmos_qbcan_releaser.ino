/* --------------------------------

This code is provided to the European Space Agency by 
Open Cosmos Ltd. for the qbcan Cansat Releaser.

The qbcan user manual can be found on http://doc.open-cosmos.com/

For queries regarding purchase of qbcan and qbcan releaser,
please contact qbshop@open-cosmos.com

Find more information about Open Cosmos at www.open-cosmos.com
Open Cosmos Ltd. 2016

-----------------------------------
*/
#include <stdlib.h>
#include <stdio.h>
#include <SimpleTimer.h>
#include <Servo.h>

// Timer Variables
//----------------------------------------------------------
//*******************************
//* Release time in miliseconds *
//*******************************
const unsigned long int release_time = 60000; // [ms]
//----------------------------------------------------------


SimpleTimer timer;
int timers_set = 0;
int timerId;
int flashId;

const int flash_period = 800;

// Servo Variables 

Servo servo1; 
const int pos1 = 180; // [deg] servo does not go above 180
const int pos2 = 12; // [deg] servo lower limit is 12

int servoState = 0;

// Pin Assignments 

const int servoPin = 10;
const int ledPin = 17; 
const int switchPin = 2; 

void activate_servo();
void close_system();
void check_switch();
void flash_led();
void set_timers();
void unset_timers();

void setup()
{  
  servo1.attach(servoPin); 

  /* "Timer active" LED */
  pinMode(ledPin, OUTPUT);

  // Timer Setup 
  set_timers();
  
  // Mechanical Switch Setup  
  pinMode(switchPin, INPUT_PULLUP);

  Serial.println("Starting");
}

void set_timers()
{
  if (timers_set) {
    unset_timers();
  }
  timerId = timer.setTimeout(release_time, activate_servo);
  flashId = timer.setInterval(flash_period / 2, flash_led);
  digitalWrite(ledPin, LOW);
  Serial.println("Timer enabled");
  timers_set = 1;
}

void unset_timers() {
  if (!timers_set) {
    return;
  }
  timer.deleteTimer(timerId);
  timer.deleteTimer(flashId);
  digitalWrite(ledPin, HIGH);
  Serial.println("Timer disabled");
  timers_set = 0;
}
 
void loop()
{  
  // Check switch position and control servo as needed
  check_switch(); 

  // Update Timer
  timer.run(); 
}

void activate_servo()
{  
  Serial.println("Opening releaser");
  servo1.write(pos1); // servo position set as (+) 180 deg 

  /* Disable the timer */
  unset_timers();

  delay(15);
}

void close_system()
{
  Serial.println("Closing releaser");
  servo1.write(pos2); // servo position set as (+) 180 deg 

  /* Enable the timer */
  set_timers();

  delay(15);
}

void flash_led()
{
  static int state = 0;
  state ^= 1;
  digitalWrite(ledPin, state ? HIGH : LOW);
}

void check_switch()
{  
  int switchState = digitalRead(switchPin);

  const int threshold = 5;
  static int hysteresis = 0;

  static int lastAction = 0;
  int action;

  /* Hysteretic state tracker, generates action if counter exceeds threshold */
  hysteresis += switchState == LOW ? 1 : -1;
  if (hysteresis > threshold) {
    hysteresis = threshold;
    action = 1;
  } else if (hysteresis < -threshold) {
    hysteresis = -threshold;
    action = -1;
  } else {
    return;
  }

  /* If action is same as last action, do nothing */
  if (action == lastAction) {
    return;
  }
  
  lastAction = action;

  if (action == 1) {
    activate_servo();
  } else if (action == -1) {
    close_system();  
  } 
     
  delay(50);
} 

