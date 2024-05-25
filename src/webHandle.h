#ifndef WEBHANDLE_H
#define WEBHANDLE_H
void web_init();
void simpan_kontrolNomer(uint8_t idx, String nomer);
void load_default();
void sendWebSerial(String txt);
void eeprom_init();

#endif