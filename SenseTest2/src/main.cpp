
/* 
 * This sketch shows how nicla can be used in standalone mode.
 * Without the need for an host, nicla can run sketches that 
 * are able to configure the bhi sensors and are able to read all 
 * the bhi sensors data.
*/

#include "Arduino.h"
#include "Arduino_BHY2.h"

SensorXYZ accel(SENSOR_ID_ACC);
SensorXYZ gyro(SENSOR_ID_GYRO);
Sensor temp(SENSOR_ID_TEMP);
Sensor gas(SENSOR_ID_GAS);
Sensor pressure(SENSOR_ID_BARO);
SensorQuaternion rotation(SENSOR_ID_RV);

void setup()
{
  Serial.begin(115200);
  while(!Serial);

  BHY2.begin();

  accel.begin();
  gyro.begin();
  temp.begin();
  gas.begin();
  rotation.begin();
  pressure.begin();
}


double pressure_to_altitude(double pressure) 
{
    const double p0 = 1013.25; // pressure at sea level

    return 44330 * (1 - pow(pressure/p0,1/5.255));
}


void loop()
{
  static auto printTime = millis();

  // Update function should be continuously polled
  BHY2.update();

  double altitude = pressure_to_altitude(pressure.value());
  Serial.println(altitude);


  //if (millis() - printTime >= 1000) {
    //printTime = millis();

    //Serial.println(String("acceleration: ") + accel.toString());
    //Serial.println(String("gyroscope: ") + gyro.toString());
    //Serial.println(String("temperature: ") + String(temp.value(),3));
    //Serial.println(String("gas: ") + String(gas.value(),3));
    //Serial.println(String("pressure: ") + String(pressure.value(),3));
    //Serial.println(String("rotation: ") + rotation.toString());
  //}

}
