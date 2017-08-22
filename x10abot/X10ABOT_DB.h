/*
  X10ABOT_DB.h - Library for flashing X10ABOT_DB code.
  Created by Rohan A. Smith, January 31, 2012.
  Released into the public domain.
*/


#ifndef X10ABOT_DB_H
#define X10ABOT_DB_H


#include "Arduino.h" //"WProgram.h"
//#include <Wire.h>

static const byte PORT_A = 1;
static const byte PORT_B = 2;
static const byte PORT_C = 3;
static const byte PORT_D = 4;
static const byte SELF = 0;

//Functions
static const byte DB_FN_IO = 1;
static const byte DB_FN_PWM  = 2;
static const byte DB_FN_ANALOG = 3;
static const byte DB_FN_SERIAL = 4;

//IO Operands
static const byte DB_OP_IO_PI_LOW = 4;  //pulse in falling edge
static const byte DB_OP_IO_PI_HI = 3;  //pulse in rising edge
static const byte DB_OP_IO_HI = 2;
static const byte DB_OP_IO_LOW = 1;
static const byte DB_OP_IO_INP = 0;

//PWM Operands
//static const byte OP_PWM_A = 0;
//static const byte OP_PWM_B = 1;

//Microcode Array
static const byte DB_FUNCTION_OPERAND = 0;
static const byte DB_D_B_SELECTION = 1;
static const byte DB_PORT_PIN = 2;
static const byte DB_SEQ_NUM = 3;
static const byte DB_INSTR_HEADER_SIZE = 4;

//port to pin assignment

typedef struct {
      byte io_pin[2];  //Input/Output pins
      byte analog; //Analog input pin
    }INPUT_PORT;

typedef struct {
      byte pwm_pin[2]; //Pulse Width Modulation pins
      byte io_pin[2]; //Input/Output pins
    }OUTPUT_PORT;

typedef struct {
      byte fn;   //function
      byte op;   //operand
      byte port; //port #
      byte port_type;
      byte db; //daughterboard address
      byte seq; //instruction sequence number
      byte data; //databyte(s)
      byte pin; //pin selector
    }MicroCode;

//free pin analog 3

class X10ABOT_DB {

  public:
    X10ABOT_DB(byte logging);
    X10ABOT_DB(byte db_address, byte logging);
    ~X10ABOT_DB();

    void receiveEvent(int numBytes);
    void requestEvent();
    void localRequest(byte * return_array);
    void localReceive(byte * message, int numBytes);
    static void receiveEvent_wrapper(int numBytes);
    static void requestEvent_wrapper();
    void execParse(MicroCode instr);
    /**
    * Logging Functions
    **/
    void i2cStatusLog(byte var);

  private:
    int _logging, _analog, _digital;
    byte _rand_mid;
    byte _lookup[6];



};

  #endif
