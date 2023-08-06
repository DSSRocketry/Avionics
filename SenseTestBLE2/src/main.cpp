//------------------------------------------------------
// Includes
//------------------------------------------------------

#include "Arduino.h"
#include "Arduino_BHY2.h"
#include "ArduinoBLE.h"
#include <math.h>

//------------------------------------------------------
// Defines
//------------------------------------------------------


#define pressureAtSealevel 1013.25f




//------------------------------------------------------
// Globals
//------------------------------------------------------

const char* sensing_service_uuid="181A"; // ID for enviromental sensing
const char* temp_characteristic_uuid="2A6E"; //ID for a temperature characteristic
const char* pressure_characteristic_uuid="2A6D"; //ID for a pressure characteristic
const char* altitude_characteristic_uuid="2A6C"; //ID for an elevation characteristic




//------------------------------------------------------
// Declarations
//------------------------------------------------------


SensorXYZ accel(SENSOR_ID_ACC);
SensorXYZ gyro(SENSOR_ID_GYRO);
Sensor temp(SENSOR_ID_TEMP);
Sensor gas(SENSOR_ID_GAS);
Sensor pressure(SENSOR_ID_BARO);
Sensor activity(SENSOR_ID_STC_HW);
SensorQuaternion rotation(SENSOR_ID_RV);




BLEService sensing_service(sensing_service_uuid);
BLEShortCharacteristic temp_characteristic(temp_characteristic_uuid, BLERead | BLEIndicate);
BLEUnsignedIntCharacteristic pressure_characteristic(pressure_characteristic_uuid, BLERead | BLEIndicate);
BLEUnsignedIntCharacteristic altitude_characteristic(altitude_characteristic_uuid, BLERead | BLEIndicate);



//------------------------------------------------------
// Functions
//------------------------------------------------------


int16_t float_to_int16(float input)
{
  input=input*100;//assume float is something like 12.34
  //Then trim to 16 bit
  if (input <-32768)
  {
    return -32768;
  } 
  else if (input >32767)
  {
    return 32767;
  }
  else
  {
    return (int16_t) round(input);
  }
}


/******************************************************************************
 * @brief Calculates the alitude from a given pressure.
 * @param pressure pressure in hPa.
 * @return the altitude of the pressure sensor in milimeters.
 ******************************************************************************/
float pressure_to_altitude(float pressure)
{
  return 100 * 44330 * (1 - pow(pressure/pressureAtSealevel,1/5.255));

}





void setup()
{
  Serial.begin(115200);
  while(!Serial);

  if (!BLE.begin()){
    Serial.println("Starting BLE failed");
    while(1);
  }


  BHY2.begin();
  accel.begin();
  gyro.begin();
  temp.begin();
  gas.begin();
  rotation.begin();
  pressure.begin();
  activity.begin();

  Serial.println("BHY Has Begun");
  BLE.setLocalName("Nicla Sense Avionics 2");
  sensing_service.addCharacteristic(temp_characteristic);
  sensing_service.addCharacteristic(pressure_characteristic);
  sensing_service.addCharacteristic(altitude_characteristic);
  BLE.addService(sensing_service);

  temp_characteristic.writeValue(0);
  pressure_characteristic.writeValue(0);
  altitude_characteristic.writeValue(0);

  BLE.advertise();
}

void loop()
{
  static auto printTime = millis();
  static float altitudevalue;
  // Update function should be continuously polled
  BHY2.update();

  if (millis() - printTime >= 1000) {
    printTime = millis();

    //Serial.println(String("Acceleration: ") + accel.toString());
    //Serial.println(String("Gyroscope: ") + gyro.toString());
    Serial.println(String("Temperature: ") + String(temp.value(),3));
    //Serial.println(String("Gas: ") + String(gas.value(),3));
    Serial.println(String("Pressure: ") + String(pressure.value()*100,3));//Pressure is in hPa by default
    Serial.println(String("Altitude: ") + String(altitudevalue,3));
    //Serial.println(String("Rotation: ") + rotation.toString());
    //Serial.println(String("activity: ") + activity.toString());
    altitudevalue=pressure_to_altitude(pressure.value());
    temp_characteristic.writeValue(float_to_int16(temp.value()));
    pressure_characteristic.writeValue((uint32_t) (pressure.value()*100));
    altitude_characteristic.writeValue((uint32_t) altitudevalue);
  }
}