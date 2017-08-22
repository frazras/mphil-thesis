#include "../X10ABOT_MB.h"
#include <math.h>
class Thermistor: public Sensor
{
  private:
    byte _db, _port;
  public:
    Thermistor(byte db_address, byte port_number):Sensor(_db, _port){
      _db = db_address;
      _port = port_number;
    }
    ~Thermistor(){};
    double readThermistorCelcius() {
        //The analog micrcode requests the raw sensor value
        int RawADC = analog(_db,_port); 
        double Temp;
        Temp = log(((10240000/RawADC) - 10000));
        Temp = 1/(0.001129148+(0.000234125+(0.0000000876741*Temp*Temp))*Temp);
        Temp = Temp - 273.15;            // Convert Kelvin to Celcius
        return Temp;
    }
    double readThermistorFarenheit() {
    // Convert Celcius to Fahrenheit
        return (readThermistorCelcius()* 9.0)/ 5.0 + 32.0;
    }
};