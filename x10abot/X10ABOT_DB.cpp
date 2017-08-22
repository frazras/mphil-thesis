/*
  X10ABOT_DB.h - Library for  X10ABOT_DB code.
  Created by Rohan A. Smith, January 31, 2012.
  Released into the public domain.
*/
#include <Wire.h>
#include "X10ABOT_DB.h" //include the declaration for this class

void* pt2Object;

//Used as the most fundamental level of coding
MicroCode instr;

//PWM pins are 3,5, 6,9, 10 & 11
//IO pin are: 2,4, 7,8, 12 & 13 (0,1 =>[rx,tx])
OUTPUT_PORT output[] = {
  //{{PWM a, PWM b}, {IO a, IO b}}
  {{10, 11}, {12, 13}},  //port 1
  {{6, 9}, {7, 8}}, //port 2
  {{3,5},{0,1}} //{0,1} =>[rx,tx])
};

//Analog pins are 0,1,2,3,4 or as Digital {14,15,16,17}
INPUT_PORT input[] = {
  //{{IO a, IO b}, analog}
  {{2, 4}, 0}, //port 1
  {{16, 17}, 1} //port 2
};


//<<constructor>>
X10ABOT_DB::X10ABOT_DB(byte db_address, byte logging){
  _logging = logging;
  Wire.begin(db_address);                // join i2c bus with address #4
 //Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent_wrapper); // register event
  Wire.onRequest(requestEvent_wrapper);
  _rand_mid = random(0, 20);
}

X10ABOT_DB::X10ABOT_DB(byte logging){
  _logging = logging;
}


//<<destructor>>
X10ABOT_DB::~X10ABOT_DB(){/*nothing to destruct*/}

/*Byte 1:1111XXXX FUNCTION BYTE
Byte 1:XXXX1111 OPERAND BYTE
Byte 2:11111111 D.B. SELECTION
Byte 3:1111111X PORT SELECTION
Byte 3:XXXXXXX1 PIN SELECTION
Byte 4:11111111 INSTRUCTION SEQUENCE NUMBER
Byte 4+n 11111111 DATA BYTES; n> 0*/


/**
* Static wrapper method for the recieve event;
**/
void X10ABOT_DB::receiveEvent_wrapper (int numBytes){
       // explicitly cast to a pointer to TClassA
       X10ABOT_DB* mySelf = (X10ABOT_DB*) pt2Object;

       // call member
       mySelf->receiveEvent(numBytes);
   }


/**
* Static wrapper method for the recieve event;
**/
void X10ABOT_DB::requestEvent_wrapper(){
       // explicitly cast to a pointer to TClassA
       X10ABOT_DB* mySelf = (X10ABOT_DB*) pt2Object;

       // call member
       mySelf->requestEvent();
   }


/**
* Implementation of the Wire/I2C recieve event
**/

void X10ABOT_DB::requestEvent(){
        Wire.write(_lookup, 7);
}

void X10ABOT_DB::localRequest(byte * return_array){
  //Serial.print("_lookup0: ");Serial.println(_lookup[0]);
  //Serial.print("_lookup1: ");Serial.println(_lookup[1]);
  for (int j = 0; j <= 5; j++){
    //Serial.print("_lookupj: ");Serial.println(_lookup[j]);
    return_array[j] = _lookup[j];
  }
}
/**
* Implementation of the Wire/I2C recieve event
**/

void X10ABOT_DB::receiveEvent(int numBytes)
{
  byte fn_op, db, port;
  while(0 < Wire.available()) // loop through all but the last
  {

    fn_op = Wire.read(); // receive FUNCTION & OPERATOR byte
    instr.db = Wire.read(); // receive Daughter Board's # byte
    port = Wire.read(); // receive Daughter Board's Port # byte
    instr.seq = Wire.read();//receive instruction sequence number

    byte op_mask = 0b00001111; // operator bitmask
    byte pin_mask =0b00000001;  //pin bitmask

    instr.fn   = fn_op  >> 4;
    instr.port = (port&127) >> 1;
    instr.port_type = port >> 7;
    instr.op   = (fn_op & op_mask);
    instr.pin  = (port & pin_mask);

    //Serial.print("instr.fn: ");Serial.println(instr.fn, BIN);
    //Serial.print("instr.op: ");Serial.println(instr.op, BIN);
    //Serial.print("instr.db: ");Serial.println(instr.db, BIN);
    //Serial.print("instr.port: ");Serial.println(port, BIN);
    //Serial.print("instr.pt: ");Serial.println(instr.port, BIN);
    //Serial.print("instr.pt_type: ");Serial.println(instr.port_type, BIN);
    //Serial.print("instr.pn: ");Serial.println(instr.pin, BIN);

    if (0<Wire.available())
    {
      while(0 < Wire.available()){ // loop through all but the last
        //Serial.println("DATA");
        instr.data = Wire.read(); // receive byte as a character
        //Serial.print("instr.data: ");Serial.println(instr.data, BIN);
        execParse(instr);      //Calls the function delegator
      }
    }
    else{
      execParse(instr);
    }
  }
}

void X10ABOT_DB::localReceive(byte * microcode, int numBytes)
{
  byte fn_op, db, port;
  if (numBytes>=DB_INSTR_HEADER_SIZE) // loop through all but the last
  {

    fn_op = microcode[DB_FUNCTION_OPERAND]; // receive FUNCTION & OPERAND byte
    instr.db = microcode[DB_D_B_SELECTION]; // receive Daughter Board's # byte
    port = microcode[DB_PORT_PIN]; // receive Daughter Board's Port # byte
    instr.seq = microcode[DB_SEQ_NUM];//receive instruction sequence number

    byte op_mask = 0b00001111; // operator bitmask
    byte pin_mask =0b00000001;  //pin bitmask

    instr.fn   = fn_op  >> 4;
    instr.port_type = port >> 7;
    instr.port = (port&255) >> 1;
    instr.op   = (fn_op & op_mask);
    instr.pin  = (port & pin_mask);
    numBytes = numBytes - DB_INSTR_HEADER_SIZE;
    //Serial.print("instr.fn: ");Serial.println(instr.fn, BIN);
    //Serial.print("instr.op: ");Serial.println(instr.op, BIN);
    //Serial.print("instr.db: ");Serial.println(instr.db, BIN);
    //Serial.print("instr.pt: ");Serial.println(instr.port, BIN);
    //Serial.print("instr.pn: ");Serial.println(instr.pin, BIN);
    //Serial.print("instr.seq: ");Serial.println(instr.seq);

    if (numBytes>DB_INSTR_HEADER_SIZE)
    {
      byte x = DB_INSTR_HEADER_SIZE;
      while(0 < numBytes){ // loop through all but the last
        //Serial.println("DATA");
        instr.data = microcode[x]; // receive byte as a character
        //Serial.print("instr.data: ");Serial.println(instr.data, BIN);
        execParse(instr);      //Calls the function delegator
        x++;
        numBytes--;
      }
    }
    else{
      execParse(instr);
    }
  }
}

void X10ABOT_DB::execParse(MicroCode instr){  //byte fn, byte op, byte db, byte port, byte data)
  int val;
  switch( instr.fn )
  {
    case DB_FN_IO:{
    //Serial.println("IO");

      switch( instr.op )
      {
      //Initialise output port
        case DB_OP_IO_HI:{
          if(instr.port_type==1){
            pinMode(input[instr.port].io_pin[instr.pin], OUTPUT);
            digitalWrite(input[instr.port].io_pin[instr.pin], HIGH);
          }else{
            pinMode(output[instr.port].io_pin[instr.pin], OUTPUT);
            digitalWrite(output[instr.port].io_pin[instr.pin], HIGH);
          }
          //Serial.print("PORT ");Serial.print(instr.port, DEC);Serial.print(" LED ");Serial.print(instr.op, DEC);Serial.print(" ON ");Serial.println(instr.pin, DEC);
          break;
        }

        case DB_OP_IO_LOW:{
          if(instr.port_type==1){
            pinMode(input[instr.port].io_pin[instr.pin], OUTPUT);
            digitalWrite(input[instr.port].io_pin[instr.pin], LOW);
          }else{
            pinMode(output[instr.port].io_pin[instr.pin], OUTPUT);
            digitalWrite(output[instr.port].io_pin[instr.pin], LOW);
          }
          //Serial.print("PORT ");Serial.print(instr.port, DEC);Serial.print(" LED ");Serial.print(instr.pin, DEC);Serial.println(" OFF");
          break;
        }

      //Initialise input port
        case DB_OP_IO_INP:{
    //case 2:
          //Serial.println(input[instr.port].io_pin[instr.pin]);
          int SensorData;
          if(instr.port_type==1){
            pinMode(input[instr.port].io_pin[instr.pin], INPUT);
            SensorData = digitalRead(input[instr.port].io_pin[instr.pin]);
          }else{
            pinMode(output[instr.port].io_pin[instr.pin], INPUT);
            SensorData = digitalRead(output[instr.port].io_pin[instr.pin]);
          }
          
          //Serial.print(F("PortType: "));Serial.println(instr.port_type);
          //Serial.print(F("PIN: "));Serial.println(input[instr.port].io_pin[instr.pin]);
          //Serial.print(F("PIN: "));Serial.println(input[instr.port].io_pin[instr.pin]);
          //Serial.print(F("SensorData: "));Serial.println(SensorData);
          itoa(SensorData,(char*)_lookup,10);

          byte z = sizeof(_lookup);
          for(int j = z - 2; j >= 0; j--) {
            _lookup[j+1] =  _lookup[j];
          }
          _lookup[0] = instr.seq;

          break;
        }
      }

      break;
    }

    case DB_FN_PWM:{
      analogWrite(output[instr.port].pwm_pin[instr.pin], instr.data);
      //Serial.print("PORT ");Serial.print(instr.port, DEC);
      //Serial.print(" PWM("); Serial.print(instr.pin); Serial.print(") DATA="); Serial.println(instr.data);
      break;
    }

    case DB_FN_SERIAL:{
      break;
    }

    case DB_FN_ANALOG:{
      int SensorData = analogRead(input[instr.port].analog);

      itoa(SensorData,(char*)_lookup,10);

      byte z = sizeof(_lookup);
      for(int j = z - 2; j >= 0; j--) {
        _lookup[j+1] =  _lookup[j];
      }
      _lookup[0] = instr.seq;
      //Serial.write(_lookup, z);
      /*for(int x=0; x<=z; x++){
        Serial.print("_lookup[]): ");  Serial.println(_lookup[x]);
        Serial.println("");
      }*/
      break;
    }
    //default:
      //Serial.print("NONE: ");
  }
}
