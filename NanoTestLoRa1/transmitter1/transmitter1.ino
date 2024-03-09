#include <SoftwareSerial.h>
 
SoftwareSerial lora(2,3);
 
int pot = A0;
 
void setup()
{
  Serial.begin(115200);
  lora.begin(115200);
  pinMode(pot, INPUT);
}
 
void loop()
{
  int val = 200;//map(analogRead(pot),0,1024,0,255);
  Serial.println(val);
  String potval = String(val);
  String cmd = "AT+SEND=0,"+String(potval.length()) +","+ String(val)+"\r";
  //Serial.println("AT+SEND=0,3,val");
  lora.println(cmd);
  while(lora.available()){
    Serial.write(lora.read());
  }
  Serial.println();
  Serial.println(cmd);
  delay(50);
}