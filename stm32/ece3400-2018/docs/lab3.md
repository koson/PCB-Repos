# Lab 3: System Integration and Radio Communication
## ECE 3400 Fall ’18

### Objective
In this lab you will integrate the components you have worked on in labs and milestones 1 and 2. You will make a robot that can start on a 660Hz tone, have it navigate a small test maze autonomously, and have it send the maze information it discovers wirelessly to a base station. The base station, in turn, will display these updates on a screen for debugging. 

Before the lab, split your team into two groups. One group will work on the radio component and the other on integrating robot start signals, line following, wall detection and detection of other robots (while ignoring decoys). For the final portion of the lab, your work will be tied together so that the robot can run through the maze and update the GUI as it explores. 

### Documentation
Throughout this lab and ALL labs, remember to document your progress on your website. Add anything that you think might be useful to the next person doing the lab. This may include helpful notes, code, schematics, diagrams, videos, and documentation of results and challenges of this lab. You will be graded on the thoroughness and readability of these websites.

Remember, all labs are mandatory; attendance will be taken at every lab. All labs will require you to split into two sub-teams, so be sure to note on the website what work is carried out by whom.

### Joint Pre-Lab
Before you come to lab, you need to discuss with your entire team how you want to encode the maze and all the information about the maze. Preferably, whatever you do for the small test setup in this lab, will apply directly to the big maze in the final challenge. We have gone over some of these principles in lectures. Remember, the final maze will have 9x9 squares. Each square can be explored/unexplored, have walls on either and all sides, have treasures (with three shapes/colors) or not, and potentially have other robots/decoys as well. Will all this information fit in the memory of the Arduino? (How much SRAM do you have available?) Consider using bit masking to compress information. 

***

## Radio Group
The radio will come in handy for debugging purposes because it acts as a (wireless) serial link between two Arduinos, one of which is hooked up to your (untethered) robot, the other to your computer. For the final challenge, to know if the robot is correctly searching through the maze, it will have to send information it discovers about the maze to a base station which will display this info using the GUI.   

### Materials
- 2 Nordic nRF24L01+ transceivers
- 2 Arduino Unos (you can borrow an extra one *for the duration of the lab session* from the TAs)
- 2 radio breakout boards with headers (if we are out, feel free to make your own from perfboards).

![Fig. 1](images/lab4_fig1.png)
![Fig. 2](images/lab4_fig2.png)

### Extra Pre-lab Assignment

This lab will use the RF24 Arduino library. Before you start the lab, look over the reference page (http://maniacbug.github.io/RF24/classRF24.html) and review the primary public interface methods at the top of the page. Make sure that you understand what they do at a high level.

You will also use the graphical user interface (GUI) prepared by the TAs. To install this GUI, please follow the instructions [here](https://github.com/backhous/ece3400-maze-gui). 

**NOTE that the radio is powered by 3.3Vmax! Do not hook it up to 5V, V_in or VCC.** It will fry!

### Procedure
**Getting Started**

Plug your radios into your Arduinos using the special printed circuit boards. Wire the radio to the **3.3V pin** on the Arduino. (NOT the 5V pin, VCC, or VIN.) 

![Fig. 3: Arduino with radio.](images/lab4_fig3.png)

Download the [RF24 Arduino library](https://github.com/maniacbug/RF24). Add it to the Libraries folder in your Arduino directory. *Note:* Do not update this library if the Arduino asks you to do so. The update will break the example code used for this lab.

Download the "Getting Started" sketch [here](https://github.com/CEI-lab/ece3400-2018/blob/master/docs/Solutions/lab4/GettingStarted/GettingStarted.ino). Replace the Getting Started code in the RF24 library example folder with the one you downloaded.

Change the identifier numbers for the two pipes to the ones assigned to your team using the following formula:

```
2(3D + N) + X
```

where D is the day of your lab (0 = Monday day, 1 = Monday night, 2 = Wednesday night, 3 = Friday, 4 = Thursday) and N is your team number. X is 0 for one radio and 1 for the other (you need 2 identifiers, which is why this X is included in the formula).

Example: You are Friday Team 9. And so, D = 3, N = 9, and your identifier numbers are then:

2*(3*3 + 9) + 0/1 = 36 and 37 in decimal = 24 and 25 in hex.

You will put these channel numbers (with leading 0s) in the line of code that says

```C
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
```

so that it instead says

```C
const uint64_t pipes[2] = { 0x0000000024LL, 0x0000000025LL };
```

The LL’s mean “long long,” or a 64-bit number. Leave these in.

Program the sketch onto *both* of your Arduinos. If you like, you can use two PC's, one for each Arduino, so that you can open a serial monitor for each simultaneously. Otherwise, you can use a single PC and switch the serial monitor between the two.

Choose one Arduino to be the transmitter and open the serial monitor for it. Type in “T” and hit enter. This will put it in Transmit mode. You should see this Arduino sending a timestamp. Switch to the serial monitor for the other Arduino and you should see it printing the received message.

Once this is working, do some quick experiments with range and channel number. How far do the radios work at the chosen power level? (*Note that power level refers to a setting on the radio, never apply more than 3V to the supply pin*) Do you have any dropped packets? Is there any interference? Is this system reliable enough to work during the final competition? Your robot will be a maximum of 15ft from your base station. 

*Note:* If you wish to try different power levels, note that the commented values are INCORRECT. The enum names are “RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, and RF24_PA_MAX.” There is no MED value that the code mentions.

Make sure you understand the "Getting Started" sketch at a high level; it is suggested that you use this sketch as a building block for your own radio code. If you have any questions, ask a TA.

**Sending Maze Information Between Arduinos**

In the final challenge the GUI will sound the 660Hz signal that starts your robot, and then log and display all the information sent to it by your robot through your base station. After 5 minutes the GUI will time out and compare what your robot has discovered to the final maze, and give you a score. Likely, your robot will not be able to finish the entire maze in time. Therefore, it is important that the robot constantly sends updates everytime it acquires new knowledge about the maze. 

* Next, you will first send wireless information over the radio. 
* You will write up code that simulates a robot moving through a maze on the extra Arduino 
* You will design a protocol for data sent between the robot and the base station, and try to send the virtual robot updates via this link. 
* You will ensure that the base station can correspond with the GUI using the correct protocol. 
* When you have finished, try it out with the real robot! 

**Simulating Your Robot**

At this point your team should have decided on the data structure they will use to encode the maze information. On your extra processor, write a program that has a pre-filled simulated maze with walls. Now, write a for-loop that runs a virtual robot through your maze (the order it runs in does not matter at this point). Every time your robot jumps to a new square in the maze, it should send information about that square wirelessly to the base station. 

You get to decide on the protocol you want to use between the robot and the base station. Think about how many characters you have to send. How big is the maximum package the Nordic Radio module can send? Can you do with a single transmission of data? Be sure to describe your chosen protocol carefully on your webpage. 

Be sure to package your code nicely, so that it easily transfers to the actual robot at the end of the lab.

Keep in mind the number of packets you expect to receive and think of a way to correct behavior if a packet is dropped. For example, if you expect to receive 8 packets but only receive 7, then when the first packet comes of another maze update, you’ll think it’s actually the 8th packet and be one off on your counting after that. The RF24 library has an Auto-ACK feature – look at the details of this and think of how enabling or disabling it would affect how you send and encode packets. (*Note:* It is enabled by default.)

**Base Station-to-GUI Transmission**

You can find lots of information about how the GUI works [here](https://github.com/backhous/ece3400-maze-gui/blob/master/README.md). Note this GUI is a work in progress, so if you find a bug, please let the TAs know immediately. It also does not have all of the graphics enabled yet. However, it should work fine for displaying walls.

Do a unit test first. I.e. check that your base station Arduino can update the GUI graphics. E.g. send information about the upper right square in a maze: Serial.println("0,0,west=true,north=true").

One this works, think about the software architecture on your base station. How often do you want to communicate with the GUI? All the time, or could you make this an event-driven operation? 

Now, integrate the full loop from the virtual robot Arduino to the base station Arduino to the GUI. Make sure that your "robot" can successfully update the entire maze.

## Robot Group

It is time to start integrating all the bits and pieces from labs 1 and 2, and milestone 1 and 2 on your robot. In this lab you will demonstrate a robot that starts on a 660Hz tone, does line following, and avoid walls. It should also stop if its path is blocked by another robot, and continue if it is just a decoy. 

### Pre-Lab

To integrate everything onboard your robot, it is a good idea to first carefully plan out the software architecture. How is your robot going to multi-task between all of these important subtasks? Are any of them more time sensitive than others? Do some sensors only have to be checked occasionally? What events could prompt these checks? It will likely help you tremendously to draw up a block diagram and maybe even a simple state machine of how you want the robot to behave. 

Like discussed briefly in lecture, you can consider event-driven operation, round-robin, timed loops, and combinations of the three. This is a hard challenge on embedded processors without multi-threaded operating systems. 

### Materials
- Your robot, all the code from the past labs 
- Decoy
- 660Hz tone generator
- Partner with another team to show robot avoidance
- Walls to make the following maze setup

![Test Maze](./images/Test-Maze.png)

### Procedure

For this lab, you should implement the simplest of all maze solving algorithms: i.e. left/right hand wall following. Because the maze is only 2x3 you can be sure that this will allow your robot to explore all the squares. You do not yet have to worry about a stop condition. Every time the robot moves to a new square it should update the data structure you agreed on with your team mates in the pre-lab.

Other than that, this is a pretty open lab, because you provide all the hardware and software. 
We have some pointers however:

* Start by doing unit tests of all of your subsystems. 
* Once you know that everything works separately, add on *one* system at a time and ensure that the new integration still works. Note, for instance, that the FFT library dependent on how it is set up, may use the same timers that the servo library which can cause trouble. 
* Write careful comments to all of your code. If you are not using Git to share code (and you really should be), it may also make sense to add initials and date to modules you have modified, so your team mates know who to contact with questions.
* Place code in separate functions to declutter the main loop. You can also move them to separate tabs in the Arduino IDE window by clicking the downward facing arrow in the upper right corner. 
* Think about what variables have to be global and which have to be local. Remember the lecture on what happens if the heap overflows.
* Debug! Now is a great time to find all of the error cases. For example, does the robot always detect walls independent of where they are? Try different maze setups and show that it still works. Does it always start independent of where the speaker is located? Does it always ignore decoys, and always stop for other robots (and start again if they're removed)? How far away do you want your robot to react to others? Even if you can't solve all of these problems in time for the lab hand-in, it is important to locate them and allocate time to deal with them before your system becomes more complicated. 

## Integration
To show that everything has been integrated, you will need to show the robot running through the maze shown above, starting on 660Hz tone, and updating the GUI on the screen as it runs. You should also show that the robot stops if you insert another robot, but continues to run if you insert a decoy instead.

### Wrap-Up

Be sure to place the extra Arduino back in the drawer before you leave. Put all tools, walls, and other components back in their appropriate bins as well. Clean up your station thoroughly before you leave!

If you decide to use any private parts for your robot, be sure to clearly label them as such. Throughout the semester we will spot check boxes, to ensure that no team has taken more than their share of parts. If your own components are not labeled as such, they will be removed. 

Use GitHub on the computer to upload and save your code, be sure to add appropriate commit messages. The lab computers will NOT keep any data locally (i.e., on them). Once you log off, the data will eventually be lost (typically overnight). Save your data on a flash drive or other means before you leave the lab.

### Grading

You need to document this lab thoroughly on your website, feel free to add ideas/comments this lab inspires regarding your future robot; the TA's will check the website by the deadline noted [here](./schedule.md). Be sure to check the [grading rubrics](./Grading/Lab_score.md) before you submit your website. Note that to encourage concise and engaging websites, we have a cap on the number of words allowed to discuss lab 3: 2,500 (not including code snippets). You can easily make up for the lack of words by adding figures, photos, and videos with captions! Remember to have a TA note your attendance before heading out.
