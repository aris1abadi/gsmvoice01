#define DEBUG_WIFI

#include <Arduino.h>
#include <webHandle.h>
#include <gsmHandle.h>

uint8_t led_dur = 2;



void setup()
{
  Serial.begin(19200); 
  eeprom_init();
  io_init();
  delay(2000);
  web_init();
  delay(5000);
  load_default();  
  gsm_init();
}

void loop()
{
  sim800_loop();
}
