/**
 * This header file provides an interface for an Arduino Uno to communicate with a Nintendo 64 controller
 * Connect the controller to GND, Data, 3V3.
 */

#ifndef N64_Interface_H
#define N64_Interface_H

/**
 * I am trying to replace this macro with a member variable inside the Interface class so we can have multiple controllers on different data pins
 * However, the overhead of accessing a member variable slows down the code and breaks the timing
 * Until I find a way to fix this, you must create ONLY ONE instance of the Interface class,
 * and you must specify the data pin here. Choose 2, 3, 4, 5, 6, or 7.
 */
#define N64_DATA_PIN 2

// status buttons1:
const char BUTTON_D_RIGHT = 0x01;
const char BUTTON_D_LEFT = 0x02;
const char BUTTON_D_DOWN = 0x04;
const char BUTTON_D_UP = 0x08;
const char BUTTON_START = 0x10;
const char BUTTON_Z = 0x20;
const char BUTTON_B = 0x40;
const char BUTTON_A = 0x80;

// status buttons2:
const char BUTTON_C_RIGHT = 0x01;
const char BUTTON_C_LEFT = 0x02;
const char BUTTON_C_DOWN = 0x04;
const char BUTTON_C_UP = 0x08;
const char BUTTON_R = 0x10;
const char BUTTON_L = 0x20;

//Controller commands
const char COMMAND_IDENTIFY = 0x00;
const char COMMAND_STATUS = 0x01;

//Structure for receiving status responses
typedef struct {
  char buttons1;
  char buttons2;
  char stick_x;
  char stick_y;
} N64_Status;

//Number of bytes in a status struct
const int status_size = sizeof(N64_Status);

//Print contents of a status response to serial
void PrintN64Status(const N64_Status& status); 

//Interface to the Nintendo 64 controller
class N64_Interface {

public:
  //Create ONLY ONE instance at this point.
  N64_Interface(); 

  //Convenience functions. You should probably use these.
  void sendStatusQuery();
  void receiveStatus(N64_Status& status);

  //Send or receive a buffer of bytes over the data wire.
  void send(char const* input, unsigned int length);
  void receive(char* output, unsigned int length);

  //Pull the data line high or low. High is idle.
  void high();
  void low();

  //Sample the data line.
  bool query();

private:
  static const char mask_ = 1<<N64_DATA_PIN; //I want to replace this with a fast member variable somehow.
  
};

#endif
