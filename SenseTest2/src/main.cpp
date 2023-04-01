#include <Arduino.h>
#include "Nicla_System.h"
int x=0;
void setup() {
  // put your setup code here, to run once:
  nicla::begin();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  x++;
  delay(1000);
}