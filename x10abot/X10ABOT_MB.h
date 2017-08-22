/*
  X10ABOT_MB.h - Library for flashing X10ABOT_MB code.
  Created by Rohan A. Smith, January 31, 2012.
  Released into the public domain.
*/


#ifndef X10ABOT_MB_H
#define X10ABOT_MB_H

#define LOGGING 0
#include <Wire.h>
#include "Arduino.h" //"WProgram.h"
//#include "Actuator.h"


//Functions
static const byte FN_IO = 1 << 4;
static const byte FN_PWM  = 2 << 4;
static const byte FN_ANALOG = 3 << 4;
static const byte FN_SERIAL = 4 << 4;


//IO Operands
static const byte OP_IO_PI_LOW = 4;  //pulse in falling edge
static const byte OP_IO_PI_HI = 3;  //pulse in rising edge
static const byte OP_IO_HI = 2;
static const byte OP_IO_LOW = 1;
static const byte OP_IO_INP = 0;

//IO Operands
static const byte HI = 2;
static const byte LO = 1;
static const byte IN = 0;

//NULL OPERATION
static const byte OP_NOP = 0;

//Ports 1 - 4
static const byte PORT_1 = 0 << 1;
static const byte PORT_2 = 1 << 1;
static const byte PORT_3 = 2 << 1;
static const byte PORT_4 = 3 << 1;

//Port Types
static const byte SENSOR= 64;
static const byte ACTUATOR = 0;

//Pins
static const byte PIN_A = 0;
static const byte PIN_B = 1;
static const byte A = 0;
static const byte B = 1;

//Microcode Array
static const byte FUNCTION_OPERAND = 0;
static const byte D_B_SELECTION = 1;
static const byte PORT_PIN = 2;
static const byte SEQ_NUM = 3;
static const byte INSTR_HEADER_SIZE = 4;


class X10ABOT_MB{

  public:
    X10ABOT_MB(byte logging);
    ~X10ABOT_MB();

    void dispatch(byte* pattern, byte byte_length);
    int requestHandler(byte* microcode, byte byte_length, byte seq_num);
    void test_function();

    //fundamental microcode operations
    void digitalOut(byte state, byte db_address, byte port_number, byte operation);
    byte digitalIn(byte db_address, byte port_number, byte operation);
    void pwm(byte pwm_select, byte db_address, byte port_number, byte duty_cycle);
    int analog(byte db_address, byte port_number);
    /**
    * Logging Functions
    **/
    void i2cStatusLog(byte var);
    void dispatchDataLog(byte * dispatch, int size);
    byte incr_instr_seq();

  private:
    int _logging;
    int _instr_seq;
    byte test_pattern[3];
    byte test_pattern2[3];
    byte _lookup[3][2];
    byte _lookup_index;
    int _analog, _digital;

};

#endif

#ifndef ACTUATOR_H
#define ACTUATOR_H

//#include "X10ABOT_MB.h"

class Actuator: public X10ABOT_MB
{

  public:
    Actuator(byte db_address, byte port_number):X10ABOT_MB(LOGGING){
      _db = db_address;
      _port = port_number+ACTUATOR;
    }
    Actuator(byte db_address, byte port_number, byte pin_select):X10ABOT_MB(LOGGING){
      _db = db_address;
      _port = port_number+ACTUATOR;
      _pin = pin_select;
    }
    //Actuator(byte db_address, byte port_number, byte pin_select);
    ~Actuator();

    void run(byte power);
    void stop();
    void on(byte power);
    void on();
    void on_a();
    void on_b();
    void pwm_a(byte power);
    void pwm_b(byte power);
    void off();
    void off_a();
    void off_b();

  private:
    byte _db, _port, _pin;
    byte _lookup[3][2];
};

#endif


#ifndef SENSOR_H
#define SENSOR_H

//#include "X10ABOT_MB.h"

class Sensor: public X10ABOT_MB
{

  public:
    Sensor(byte db_address, byte port_number):X10ABOT_MB(LOGGING){
      _db = db_address;
      _port = port_number+SENSOR;
    }
    Sensor(byte db_address, byte port_number, byte pin_select):X10ABOT_MB(LOGGING){
      _db = db_address;
      _port = port_number+SENSOR;
      _pin = pin_select;
    }
    //Sensor(byte db_address, byte port_number, byte pin_select);
    ~Sensor();
    byte readDigital();
    void writeDigitalLo();
    void writeDigitalHi();
    void readDigitalA();
    void readDigitalB();
    byte readDigital(byte power);
    int readAnalog();
    int getAnalog();
    void off();

  private:
    byte _db, _port,_pin;
    int _analog, _digital;
};

#endif