#include <Arduino.h>
 
int LED = 5;
//String inString = "";    // string to hold input
int val = 0;

HardwareSerial lora = Serial1;


void setup()
{
  Serial.begin(115200);
  lora.begin(115200);

  delay(500);

  lora.println("AT/r/n");
  delay(100);
  while(lora.available()){
    //Serial.println(String("In available"));
    Serial.write(lora.read());
  }
  Serial.println(String("Done"));
}
 
void loop()
{
  if (Serial.available()) {
      String input = Serial.readStringUntil('\n');
      Serial.println("Received input: " + input);
      input.trim();                        // remove any \r \n whitespace at the end of the String
      delay(10);
      lora.println(input);
    }

    // Check for LoRa availability in the background
    if (lora.available()) {
      // Process LoRa data as desired
      Serial.write(lora.read());
    }
}