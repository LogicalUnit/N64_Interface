/**
 * Author: John Crain Welsby (john.welsby@gmail.com) in collaboration with Arduino community, especially Nick Gammon.
 * Date: October 2016
 * Version: 1.0
 * 
 * Implementation of N64_Interface. Specifically designed to run on 16MHz 5V Arduino Uno, or compatible.
 */


#include "N64_Interface.h"
#include "Arduino.h"
#include <avr/wdt.h> //For watchdog


//Do nothing for 1 cycle, 1/16 of a microsecond on Arduino Uno
#define NOP asm volatile ("nop\n\t")


N64_Interface::N64_Interface(int data_pin) {
  mask_ = 1<<data_pin;
  digitalWrite(data_pin, LOW); //do not make HIGH! This will fry the controller!
  high(); //high means idle
}


void N64_Interface::send(char const* input, int length) {
 
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

void N64_Interface::receive(char* output, int length) {

  char mask = mask_; //Create a local copy for speed reasons
  
  wdt_reset();  
  wdt_enable(WDTO_30MS); //timeout after 30ms
  
  for (int i = 0; i < length; i++) {
    char currentByte = 0;

    for (int j = 0; j < 8; j++) {
      while(PIND & mask); //wait for low
   
      //wait 1 microsecond
      NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;

      bool currentBit = (PIND & mask) != 0; //query the line
      currentByte <<= 1; //shift left
      currentByte |= currentBit; //add latest bit

      while(!(PIND & mask)); //wait for high
    }

    output[i] = currentByte;
  }

  //wait for stop bit
  while(PIND & mask);
  while(!(PIND & mask));

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
  if (status.buttons2 &  BUTTON_L)
    Serial.print (F("L "));
  if (status.buttons2 &  BUTTON_R)
    Serial.print (F("R "));
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
  if (status.buttons1 &  BUTTON_A)
    Serial.print (F("A "));
  if (status.buttons1 &  BUTTON_B)
    Serial.print (F("B "));
  if (status.buttons1 &  BUTTON_Z)
    Serial.print (F("Z "));
 

  if (status.buttons1 == 0 && status.buttons2 == 0)
    Serial.print (F("(no buttons)"));

  Serial.println();
  Serial.flush();
}

void N64_Interface::high()
{
  DDRD &= ~mask_; //Make data pin an input (high impedence, simulates high)
}

void N64_Interface::low()
{
  DDRD |= mask_; //Make data pin an output (low)
}


