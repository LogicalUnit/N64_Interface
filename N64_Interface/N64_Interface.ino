/**
 * Author: John Crain Welsby (john.welsby@gmail.com)
 * Date: October 2016
 * Version: 1.0
 * 
 * Simple demonstration of communication with a Nintendo 64 controller.
 * Controller must be connected as described in N64_Interface.h (GND, Data, 3V3).
 * Serial output at 115200 baud.
 */
 
#include "N64_Interface.h"

//Global variables
N64_Interface interface(2); //N64 controller Data wire is connected to pin 2.
N64_Status status, oldStatus; //Status of joystick and buttons.

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("Starting");

  //Start by clearing the status variables
  memset(&status, 0, status_size);
  memset(&oldStatus, 0, status_size);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  noInterrupts(); //Disable interrupts, they interfere with timings.
  interface.sendStatusQuery(); //This is how we send the status query (1 byte).
  interface.receiveStatus(status); //This is how we receive the status response (4 bytes).
  interrupts(); //Timing critical section complete, re-enable interrupts.

  if (memcmp(&status, &oldStatus, status_size)) { //If the status has changed since last update.
    PrintN64Status(status); //Print status to serial.
    oldStatus = status; //Remember previous status.
  }

  delay(50); //Delay 50ms, about 20 updates per second.
}
