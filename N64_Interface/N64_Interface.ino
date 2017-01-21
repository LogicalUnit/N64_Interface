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
N64_Interface *interface; //Pointer to main interface.
N64_Status status, old_status; //Status of joystick and buttons.

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.println("Starting");

  //Start by clearing the status variables.
  memset(&status, 0, status_size);
  memset(&old_status, 0, status_size);

  //Disable interrupts, they interfere with timings on pin 2
  noInterrupts(); 

  //N64 controller Data wire is connected to pin 2.
  interface = new N64_Interface(2);   
}

void loop() {
  // put your main code here, to run repeatedly:
  
  interface->send(&COMMAND_STATUS, 1); //This is how we send the status query.
  interface->receive((char*)&status, status_size); //This is how we receive the response.

  if (memcmp(&status, &old_status, status_size)) { //If the status has changed since last update.
    PrintN64Status(status); //Print status to serial.
    memcpy(&old_status, &status, status_size); //Remember previous status.
  }

  delay(50); //Delay 50ms, about 20 updates per second.
}
