#include "X10ABOT_MB.h"
/*
Byte 1:1111XXXX FUNCTION BYTE
Byte 1:XXXX1111 OPERAND BYTE
Byte 2:11111111 D.B. SELECTION
Byte 3:1111111X PORT SELECTION
Byte 3:XXXXXXX1 PIN SELECTION
Byte 4:11111111 INSTRUCTION SEQUENCE NUMBER
Byte 4+n 11111111 DATA BYTES; n> 0
*/

/**
 * Asserts a state to a Digital IO pin
 *
 * @param  byte  state  The state of the digital pin, HIGH(2), LOW(1), INPUT(0)
 * @param  byte  db_address The daughterboard address between 7 and 120, 0 for motherboard
 * @param  byte  port_number the daughter board's port number connected to the device
 * @param  byte  pin_select choose which of the I/O pins on the port to use A(0) or B(1)
 * @return void
 */
void X10ABOT_MB::digitalOut(byte state, byte db_address, byte port_number, byte pin_select){
  byte microcode[] =   {FN_IO+state,db_address,((port_number-1)<<1)+pin_select,incr_instr_seq()};
  dispatch(microcode, sizeof(microcode));

}
byte X10ABOT_MB::digitalIn(byte db_address, byte port_number, byte pin_select){
  byte seq_num = incr_instr_seq();
  byte microcode[] =   {FN_IO+IN,db_address,((port_number-1)<<1)+pin_select,seq_num};
  dispatch(microcode, sizeof(microcode));
  //delay(50);
   return requestHandler(microcode, 2,seq_num);
}
/**
 * Asserts a PWM signal to a pin
 *
 * @param  byte  pwm_select choose which of the PWM pins on the port to use A(0) or B(1)
 * @param  byte  db_address The daughterboard address between 7 and 120, 0 for motherboard
 * @param  byte  port_number the daughter board's port number connected to the device
 * @param  byte  duty_cycle specify the duty cycle as a number between 0 and 255
 * @return void
 */
void X10ABOT_MB::pwm(byte pwm_select, byte db_address, byte port_number, byte duty_cycle){
  byte microcode[] =   {FN_PWM,db_address,((port_number-1)<<1)+pwm_select,incr_instr_seq(),duty_cycle};
  dispatch(microcode, sizeof(microcode));
}

/**
 * Reads the state from an Analogue pin
 *
 * @param  byte  db_address The daughterboard address between 7 and 120, 0 for motherboard
 * @param  byte  port_number the daughter board's port number connected to the device
 * @return int
 */
int X10ABOT_MB::analog(byte db_address, byte port_number){
  byte seq_num = incr_instr_seq();
  byte microcode[] =   {FN_ANALOG,db_address,((port_number-1)<<1),seq_num };
  dispatch(microcode, sizeof(microcode));
  //delay(50);
  return requestHandler(microcode, 6,seq_num);
}
