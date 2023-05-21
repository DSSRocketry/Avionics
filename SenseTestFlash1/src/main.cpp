/* Example for blinking and sensor reading in two RTOS threads*/
//Include the relevant libraries
//Note that the BHY2 library requires the arduino BLE library to be
//available in the project as well
#include "Arduino.h"
#include "Nicla_System.h"
#include "Arduino_BHY2.h"
#include "SPIFBlockDevice.h"
#include "MBRBlockDevice.h"


#define PARTITION_TYPE 0x0B // FAT 32
#define BLOCK_DEVICE_SIZE 1024 * 1024 * 2 // 2 MB

//This namespace allows use of time in simple formats
// such as 1s for 1 second
using namespace std::chrono_literals;


//declare the sensors
SensorXYZ accel(SENSOR_ID_ACC);
SensorXYZ gyro(SENSOR_ID_GYRO);
Sensor temp(SENSOR_ID_TEMP);
Sensor gas(SENSOR_ID_GAS);
Sensor pressure(SENSOR_ID_BARO);
SensorQuaternion rotation(SENSOR_ID_RV);

//declare the threads

rtos::Thread blinkthread(osPriorityAboveNormal);//heartbeat blink is wanted
rtos::Thread sensorupdatethread;//not actually used currently
rtos::Thread sensorprintthread;//sensor updating will happily consume all time it seems
//some testing remains with the thread priority. below normal seems to cause threads to not run

/**function to run in a thread and blink the onboard led
 */
void blink_app(void)
{
  while(1) {
    nicla::leds.setColor(green);                       //turn green LED on
    rtos::ThisThread::sleep_for(1s);                   //wait 1 second
    nicla::leds.setColor(off);                         //turn all LEDs off
    rtos::ThisThread::sleep_for(1s);                   //wait 1 second
  }
}

/**function to run in a thread and update sensor data.
 * You want to update it constantly
 */
void sensorupdate_app(void){
  BHY2.update();

}




/**function to run in a thread and print sensor data to the serial port
 * you only want to print every second
 */
void sensorprint_app(void)
{

  while(1) {
    BHY2.update();
    Serial.println(String("acceleration: ") + accel.toString());
    Serial.println(String("gyroscope: ") + gyro.toString());
    Serial.println(String("temperature: ") + String(temp.value(),3));
    Serial.println(String("gas: ") + String(gas.value(),3));
    Serial.println(String("pressure: ") + String(pressure.value(),3));
    Serial.println(String("rotation: ") + rotation.toString());
    Serial.println(millis());
    rtos::ThisThread::sleep_for(1s);                   //wait 1 second
  }
}

void setup(void)
{
  //run this code once when Nicla Sense ME board turns on
  nicla::begin();               // initialise library
  nicla::leds.begin();          // Start I2C connection
  Serial.begin(115200);
  while(!Serial);
  Serial.println(String("log msg 1")); //example of sending a message to debug with serial


  Serial.println("SPI Flash Device Test");
  Serial.println("------------------------"); 


  randomSeed(analogRead(A0)); // seed generator for later random value

  SPIFBlockDevice extflash; //the default values in the constructor are all good
  mbed::MBRBlockDevice blockDevice(&extflash, 1);  

  // Initialize the Flash IAP block device and print the memory layout
  if(blockDevice.init() != 0 || blockDevice.size() != BLOCK_DEVICE_SIZE) {    
    Serial.println("Partitioning block device...");
    blockDevice.deinit();
    // Allocate a FAT 32 partition
    mbed::MBRBlockDevice::partition(&extflash, 1, PARTITION_TYPE, 0, BLOCK_DEVICE_SIZE);
    blockDevice.init();
  }
  
  const auto eraseBlockSize = blockDevice.get_erase_size();
  const auto programBlockSize = blockDevice.get_program_size();

  Serial.println("Block device size: " + String((unsigned int) blockDevice.size() / 1024) + " KB");  
  Serial.println("Readable block size: " + String((unsigned int) blockDevice.get_read_size())  + " bytes");
  Serial.println("Programmable block size: " + String((unsigned int) programBlockSize) + " bytes");
  Serial.println("Erasable block size: " + String((unsigned int) eraseBlockSize / 1024) + " KB");
     
  String newMessage = "Random number: " + String(random(1024));
  
  // Calculate the amount of bytes needed to store the message
  // This has to be a multiple of the program block size
  const auto messageSize = newMessage.length() + 1; // C String takes 1 byte for NULL termination
  const unsigned int requiredEraseBlocks = ceil(messageSize / (float)  eraseBlockSize);
  const unsigned int requiredBlocks = ceil(messageSize / (float)  programBlockSize);
  const auto dataSize = requiredBlocks * programBlockSize;  
  char buffer[dataSize] {};  


  //Note that, because how how big an area is being formatted, there are bytes at the being likely
  //That are erased as part of the partioning. So we have to read from an address further down than 0


  // Read back what was stored at previous execution  
  Serial.println("Reading previous message...");
  blockDevice.read(buffer, 1024*8, dataSize);
  Serial.println(buffer);

  // Erase a block starting at the offset 0 relative
  // to the block device start address
  blockDevice.erase(1024*8, requiredEraseBlocks * eraseBlockSize);

  // Write an updated message to the first block
  Serial.println("Writing new message...");
  Serial.println(newMessage);  
  blockDevice.program(newMessage.c_str(), 1024*8, dataSize);
  


  blockDevice.deinit();
  Serial.println("Done.");
  rtos::ThisThread::sleep_for(5s);                   //wait 5 seconds

  // Start the sensor library and sensors
  BHY2.begin();
  accel.begin();
  gyro.begin();
  temp.begin();
  gas.begin();
  rotation.begin();
  pressure.begin();



















  blinkthread.start(blink_app);
  //sensorupdatethread.start(sensorupdate_app);
  //sensorprintthread.start(sensorprint_app);
  blinkthread.join(); //wait for blink to end (aka never return to main)
}


void loop(void){
  
}

