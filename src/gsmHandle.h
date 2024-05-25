#ifndef GSMHANDLE_H
#define GSMHANDLE_H

void io_init();
void gsm_init();
void sim800_loop();
void setRingVol(String v);
void setVoiceVol(String r);
void setRingGsm(String vol);
void setVoiceGsm(String vol);

String getRingVol();
String getVoiceVol();

void kirimAt(String at);

#endif