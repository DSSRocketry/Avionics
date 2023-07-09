#include <Arduino.h>

HardwareSerial lora = Serial1;
 
//int pot = A0;
 
void setup()
{
  Serial.begin(115200);
  lora.begin(115200);
  //pinMode(pot, INPUT);
  delay(500);
  //Serial.println(String("In setup"));
  //String cmd = "AT+SEND=0,"+String(potval.length()) +","+ String(val)+"\r";
  String cmd = "AT+ADDRESS=1\r\n";
  //Serial.println("AT+SEND=0,3,val");
  lora.println(cmd);
  delay(500);
  cmd = "AT+NETWORKID=1\r\n";
  //Serial.println("AT+SEND=0,3,val");
  lora.println(cmd);
  delay(500);
  while(lora.available()){
    //Serial.println(String("In available"));
    Serial.write(lora.read());
  }
  Serial.println(String("Done"));


}
 
void loop()
{
  //int val = map(analogRead(pot),0,1024,0,255);
  //Serial.println(cmd);
  delay(50);
}

