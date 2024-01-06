# Laser Golf Hole

## Background

I have a OptiShot 2 golf simulator and found that putting was a pain. There was no visual representation of the hole. This is designed to help solve this issue by showing a red dot where the hole is relative to the putter to assist in more accurate putting.


## Info

This is a basic set of code using some trig to determine the angle to set the laser. This assumes that the laser is hanging from the Celing directly above the ball.

You can interact with the device either over serial or by going to the ip address of the 

### Hardware

1) I am using an ESP32 (Haltec esp32 V1) but any esp32 should work
2) Two SG90 Servos
3) This 3D Printed PanTilt module <https://www.thingiverse.com/thing:4710301>
4) An Arduino Laser module.

#### Wireing Diagram

### Code

The code is written in C++ using Arduino Libraries and Platform.io as the environment. Also using Visual Studio code as the IDE.

#### Deploy

1) Run platform io build
2) plug in the esp32
3) Build Filesystem Image -- To Include the html page in spiff
4) Upload Filesystem Image
5) Run platform io deploy


##### Note

You may need to change the board in the README.md to match your specfic board. Boards can be found at platform io.

### Setup 

