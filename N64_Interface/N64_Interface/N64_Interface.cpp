#include "N64_Interface.h"
#include "Arduino.h"
#include <avr/wdt.h> //For watchdog

//Send a logical high to the controller
#define N64_HIGH    (DDRD &= ~(1<<N64_PIN))

//Send a logical low to the controller
#define N64_LOW     (DDRD |= (1<<N64_PIN))

//Query the state of the Data pin
#define N64_QUERY   ((PIND & (1<<N64_PIN)) != 0)

//Do nothing for 1 cycle, 1/16 of a microsecond on Arduino Uno
#define NOP         asm volatile ("nop\n\t")


N64_Interface::N64_Interface(int data_pin){
  data_pin_ = data_pin;
  pinMode(data_pin_, INPUT); // do not make INPUT_PULLUP! This will fry the controller!
  digitalWrite(data_pin_, LOW); //do not make HIGH! This will fry the controller!
}


void N64_Interface::send(char const* input, unsigned int length) {
 
  for (int i = 0; i < length; i++) {
    char currentByte = input[i];

    for (int j = 0; j < 8; j++) {
      bool currentBit = currentByte & 0x80; //get leftmost bit
      currentByte <<= 1; //shift left

      if (currentBit) {
        //send logical 1
        low();
        NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
        high();
        NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
        NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
        NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
        } else {
        //send logical 0
        low();
        NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
        NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
        NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
        high();
        NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
      }
    }
  }

  //send stop bit
  low();
  NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
  high();
}

void N64_Interface::receive(char* output, unsigned int length) {

  wdt_reset();  //pat the dog
  wdt_enable(WDTO_30MS); //timeout after 30ms
  
  for (int i = 0; i < length; i++) {
    char currentByte = 0;

    for (int j = 0; j < 8; j++) {
      while (query()); //wait for start

      //wait 2 microseconds
      NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;

      currentByte <<= 1; //shift left
      currentByte |= query(); //add latest bit

      while(!query()); //wait for end
    }

    output[i] = currentByte;
  }

  //wait for stop bit
  while(query());
  while(!query());

  wdt_disable();  
}


void PrintN64Status(const N64_Status& status)
{
  Serial.print (F("X: "));
  Serial.print ((int) status.stick_x);
  Serial.print (F(" "));

  Serial.print (F("Y: "));
  Serial.print ((int) status.stick_y);
  Serial.print (F(" "));

  if (status.buttons2 &  BUTTON_C_RIGHT)
    Serial.print (F("C right "));
  if (status.buttons2 &  BUTTON_C_LEFT)
    Serial.print (F("C left "));
  if (status.buttons2 &  BUTTON_C_UP)
    Serial.print (F("C up "));
  if (status.buttons2 &  BUTTON_C_DOWN)
    Serial.print (F("C down "));
  if (status.buttons2 &  BUTTON_R)
    Serial.print (F("R "));
  if (status.buttons2 &  BUTTON_L)
    Serial.print (F("L "));
  if (status.buttons1 &  BUTTON_D_RIGHT)
    Serial.print (F("D right "));
  if (status.buttons1 &  BUTTON_D_LEFT)
    Serial.print (F("D left "));
  if (status.buttons1 &  BUTTON_D_UP)
    Serial.print (F("D up "));
  if (status.buttons1 &  BUTTON_D_DOWN)
    Serial.print (F("D down "));
  if (status.buttons1 &  BUTTON_START)
    Serial.print (F("Start "));
  if (status.buttons1 &  BUTTON_Z)
    Serial.print (F("Z "));
  if (status.buttons1 &  BUTTON_B)
    Serial.print (F("B "));
  if (status.buttons1 &  BUTTON_A)
    Serial.print (F("A "));

  if (status.buttons1 == 0 && status.buttons2 == 0)
    Serial.print (F("(no buttons)"));

  Serial.println();
  Serial.flush();
}

void N64_Interface::sendStatusQuery()
{
  send(&COMMAND_STATUS, 1);
}

void N64_Interface::receiveStatus(N64_Status& status)
{
  receive((char*)&status, status_size);
}

void N64_Interface::high()
{
  (DDRD &= ~(1<<data_pin_));
}

void N64_Interface::low()
{
  (DDRD |= (1<<data_pin_));
}

bool N64_Interface::query()
{
  return (PIND & (1<<N64_PIN)); //this works
  //return (PIND & (1<<data_pin_)); //this doesn't
}

