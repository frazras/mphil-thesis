/**
 * X10ABOT_MB.h - Library for flashing X10ABOT_MB code.
 * Created by Rohan A. Smith, January 31, 2012.
 * Released into the public domain.
 *
 * Long description for class (if any)...
 *
 * @copyright  2013 Rohan Smith
 * @license    http://www.zend.com/license/3_0.txt   PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://dev.zend.com/package/PackageName
 * @since      Class available since Release 1.2.0
 */

/*
  X10ABOT_MB.h - Library for flashing X10ABOT_MB code.
  Created by Rohan A. Smith, January 31, 2012.
  Released into the public domain.
*/

#include "X10ABOT_MB.h" //include the declaration for this class
#include <X10ABOT_DB.h> //include the declaration for this class
#include <Wire.h>
  //#include <MemoryFree.h>

X10ABOT_DB db(LOGGING);


/**
 * X10ABOT_MB Constructor
 *
 * @param  byte logging  Toggles logging over Serial ON(1) OFF(0)
*/
X10ABOT_MB::X10ABOT_MB(byte logging){
  _logging = logging;
  _instr_seq = 0;
  _lookup_index = 0;
}

/**
 * X10ABOT_MB Destructor
 * no params
 */
X10ABOT_MB::~X10ABOT_MB(){
  /*nothing to destruct*/
}

/**
 * X10ABOT_MB Destructor
 * no params
 */
byte X10ABOT_MB::incr_instr_seq(){
  if (_instr_seq>250)
  {
    _instr_seq = 0;
  }else{
    _instr_seq = _instr_seq+1;
  }
  return _instr_seq;
}

/**
 * Dispatches microcode to the internal/external daughterboard for execution
 *
 * @param  byte*  microcode  This is an array of microcode instructions to be sent
 * @param  byte   byte_length the size in bytes of the microcode instruction
 * @return void
 */
void X10ABOT_MB::dispatch(byte* microcode, byte byte_length){
  if (microcode[D_B_SELECTION]==0){
    db.localReceive(microcode, byte_length);
  }
  else{
   Wire.begin();
   // Read and write here to 400kHz devices
   //((16000000MHz/400000L)-16)/2 =
   //TWBR=0x0C; // (DEC = 12) 400000L
   TWBR=0x13; // (DEC = 19) ~300000L
   //TWBR = 0x20; // (DEC = 32) 200000L
   //TWBR=0x48; // (DEC = 72) DEFAULT 100000L

    Wire.beginTransmission(microcode[D_B_SELECTION]); // transmit to device #x
    Wire.write(microcode, byte_length);              // sends all bytes
    i2cStatusLog(Wire.endTransmission());    // stop transmitting
  }
  dispatchDataLog(microcode, sizeof(microcode));
}

/**
 * Sends requests using  microcode to the internal/external daughterboard for execution
 *
 * @param  byte   byte_length the size in bytes of the microcode instruction
 * @return void
 */
int X10ABOT_MB::requestHandler(byte* microcode, byte byte_length, byte seq_num){
  for (int i = 0; i < 3; ++i){
    if (_lookup[i][0]==seq_num){
      return _lookup[i][1];
    }

  }
  if (microcode[D_B_SELECTION]==0){
    byte return_array[6];
    db.localRequest(return_array);
    //Serial.print("return_array0: ");Serial.println(return_array[0]);
    //Serial.print("return_array1: ");Serial.println(return_array[1]);
    //Serial.print("seq#: ");Serial.println(seq_num);

    //_lookup[_lookup_index][0]=return_array[0];
    //_lookup[_lookup_index][1]=return_array[1];
    byte output[5];
    if (return_array[0]!=seq_num){
      
         //loop into cache
      _lookup[_lookup_index][0]= return_array[0];
        //for (int j = 0; j < 1; j++){
      _lookup[_lookup_index][1]=return_array[1];
      if (_lookup_index>2){
        _lookup_index = 0;
      }else{
        _lookup_index++;
      }
    }else{
      
      for (int y=0; y<6;y++){
        output[y]=return_array[y+1];
      }//Serial.println(freeMemory());Serial.println(' ');
      int x = atoi((char*)output);
      return x;
    }
  }
  else{
    //Wire.begin();
    //Wire.beginTransmission(microcode[D_B_SELECTION]); // transmit to device #x
    //Wire.write(microcode, byte_length);              // sends all bytes
    //i2cStatusLog(Wire.endTransmission());    // stop transmitting
     // Read and write here to 400kHz devices
   //((16000000MHz/400000L)-16)/2 =
   //TWBR=0x0C; // (DEC = 12) 400000L
   //TWBR=0x13; // (DEC = 19) ~300000L
    TWBR = 0x1B; // (DEC = 27) ~230000L
   //TWBR = 0x20; // (DEC = 32) 200000L
   //TWBR=0x48; // (DEC = 72) DEFAULT 100000L

    Wire.requestFrom((int)microcode[D_B_SELECTION], 6);
    if(Wire.available()){    // slave may send less than requested
      byte c = Wire.read();
      if (c!=seq_num){
        //Serial.print(c);
        //Serial.print(" UNEQUAL TO ");
        //Serial.print(seq_num);
         //loop into cache
        _lookup[_lookup_index][0]= c;
        //for (int j = 0; j < 1; j++){
        _lookup[_lookup_index][1]=Wire.read();
        if (_lookup_index>2){
            _lookup_index = 0;
          }else{
            _lookup_index++;
          }
        }else{
        //DO SOMETHING WITH c
          byte result[5];
          byte x=0;
        while(Wire.available()){    // slave may send less than requested
          result[x]=Wire.read();
//Serial.print("Result: ");Serial.println(result[x]);
          x++;
        }
        int w = atoi((char*)result);
        //Serial.print("Source SensorData X: ");Serial.println(w);
        return w;
      }
    }

    dispatchDataLog(microcode, sizeof(microcode));
  }
}

