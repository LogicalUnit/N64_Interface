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
N64_Interface *interface; 
N64_Status status, old_status; //Status of joystick and buttons.

void setup() {
  // put your setup code here, to run once:

  //Disable interrupts, they interfere with timings.
  noInterrupts(); 

  //Start by clearing the status variables.
  memset(&status, 0, status_size);
  memset(&old_status, 0, status_size);

  //N64 controller Data wire is connected to pin 2.
  interface = new N64_Interface(2); 
  
  Serial.begin(115200);
  Serial.println("Starting");
}

void loop() {
  // put your main code here, to run repeatedly:
  
  interface->getStatus(status); //This is how we get the status response.

  if (memcmp(&status, &old_status, status_size)) { //If the status has changed since last update.
    PrintN64Status(status); //Print status to serial.
    old_status = status; //Remember previous status.
  }

  delay(50); //Delay 50ms, about 20 updates per second.
}
