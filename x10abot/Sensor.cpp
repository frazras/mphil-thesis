#include "X10ABOT_MB.h"
//#include <MemoryFree.h>
/**
 * Sensor Constructor
 *
 * @param  byte db_address   Sets the  daughterboard's address
 * @param  byte port_number  Sets the  port_number on the daughterboard specified
 */
//Sensor::Sensor(byte db_address, byte port_number)

/**
 * Sensor Constructor
 *
 * @param  byte db_address  Sets the  daughterboard's address
 * @param  byte port_number  Sets the  port_number on the daughterboard specified
 * @param  byte pin_select  Selects either pin A(0) or B(1) on the selected port
 */
/*Sensor::Sensor(byte db_address, byte port_number, byte pin_select){
    _db = db_address;
    _port = port_number;
    _pin = pin_select;
}
*/
/**
 * Sensor Destructor
 * no params
 */
Sensor::~Sensor(){
  /*nothing to destruct*/
}

//X10ABOT_MB sensor(LOGGING);

/**
 * Sensor readDigital
 *
 * @param  byte power  Drives the Sensor in the negative or positive direction based on the power level. It operates btween -100 and +100.
 */

byte Sensor::readDigital(){
   byte x =  digitalIn(_db, _port, _pin);
   return x;
 }

void Sensor::writeDigitalHi(){
 digitalOut(HI, _db, _port, _pin);
}

void Sensor::writeDigitalLo(){	
 digitalOut(LO, _db, _port, _pin);
}

int Sensor::readAnalog(){
  return analog(_db,_port);
 }

void Sensor::off(){}
void Sensor::readDigitalA(){}
void Sensor::readDigitalB(){}
