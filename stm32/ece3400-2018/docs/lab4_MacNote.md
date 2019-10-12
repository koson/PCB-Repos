# ECE 3400 Intelligent Physical Systems
## Cornell University, Fall 2018
...by Tyler Sherman

Mac Instructions for Lab 4

In Lab 4, it states: "Every time you start a new lab session, *BEFORE* uploading ANY code to your Arduino that includes what's in the template, you have to disable the internal pull-up resistors that are a part of the Arduino's I2C interface. This is because they pull the signals that set up our camera to 5V, while our camera requires 3.3V. Sending 5V through will harm the camera." The directions included only work for Windows. To do the same thing on Mac, the instructions are:

* Go to your applications folder and right-click on Arduino
* Click "Show Package Contents"
* Enter the "Contents" folder
* Enter the "Java" folder
* Enter the "hardware" folder
* Enter the "Arduino" folder
* Enter the "avr" folder
* Enter the "libraries" folder
* Enter the "Wire" folder
* Enter the "src" folder
* Enter the "utility" folder
* Right-click on "twi.c" and select "Get Info"
* Under "Sharing & Permissions," make sure your privilege settings are "Read & Write"
* Open "twi.c"
* Comment out the below lines, then save & close


```C
  //activate internal pullups for twi
  digitalWrite(SDA,1);
  digitalWrite(SCL,1);
  
```
