#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>
#include <FS.h>
#include <EEPROM.h>
#include <gsmHandle.h>

AsyncWebServer server(80);

#define EE_NOMER_1 10
#define EE_NOMER_2 50
#define EE_NOMER_3 100

extern String kontrolNomer1;
extern String kontrolNomer2;
extern String kontrolNomer3;

const char *ssid = "Roypro Walkie-talkie";
const char *passwordAP = "roypro123";

const char *RING_VOL = "ring";
const char *VOICE_VOL = "voice";

void recvMsg(uint8_t *data, size_t len)
{
    // WebSerial.println("Received Data...");
    String d = "";
    for (int i = 0; i < len; i++)
    {
        d += char(data[i]);
    }
    // WebSerial.println(d);
    kirimAt(d);
}

// Replaces placeholder with button section in your web page
String processor(const String &var)
{
    // Serial.println(var);
    if (var == "RINGVALUE")
    {
        return getRingVol();
    }
    else if (var == "VOICEVALUE")
    {
        return getVoiceVol();
    }
    else if (var == "RINGVALUETEXT")
    {
        String txt = "Volume Ring ";
        txt += getRingVol();
        return txt;
    }
    else if (var == "VOICEVALUETEXT")
    {
        String txt = "Volume Speaker ";
        txt += getVoiceVol();
        return txt;
    }
    else if (var == "NOMER_1")
    {
        return kontrolNomer1;
    }
    else if (var == "NOMER_2")
    {
        return kontrolNomer2;
    }
    else if (var == "NOMER_3")
    {
        return kontrolNomer3;
    }
    return String();
}

void simpan_kontrolNomer(uint8_t idx, String nomer)
{
    if (idx == 1)
    {
        if (nomer.length() != 0)
            EEPROM.write(EE_NOMER_1, 0xAA);
        EEPROM.write(EE_NOMER_1 + 1, nomer.length());
        for (uint8_t i = 0; i < nomer.length(); i++)
        {
            EEPROM.write(EE_NOMER_1 + 2 + i, nomer.charAt(i));
        }
        EEPROM.commit();

        WebSerial.print("Nomer ");
        WebSerial.print(nomer);
        WebSerial.print(" disimpan di index ");
        WebSerial.println(idx);
    }
    else if (idx == 2)
    {
        if (nomer.length() != 0)
            EEPROM.write(EE_NOMER_2, 0xAA);
        EEPROM.write(EE_NOMER_2 + 1, nomer.length());
        for (uint8_t i = 0; i < nomer.length(); i++)
        {
            EEPROM.write(EE_NOMER_2 + 2 + i, nomer.charAt(i));
        }
        EEPROM.commit();

        WebSerial.print("Nomer ");
        WebSerial.print(nomer);
        WebSerial.print(" disimpan di index ");
        WebSerial.println(idx);
    }
    else if (idx == 3)
    {
        if (nomer.length() != 0)
            EEPROM.write(EE_NOMER_3, 0xAA);
        EEPROM.write(EE_NOMER_3 + 1, nomer.length());
        for (uint8_t i = 0; i < nomer.length(); i++)
        {
            EEPROM.write(EE_NOMER_3 + 2 + i, nomer.charAt(i));
        }
        EEPROM.commit();

        WebSerial.print("Nomer ");
        WebSerial.print(nomer);
        WebSerial.print(" disimpan di index ");
        WebSerial.println(idx);
    }
}

void sendWebSerial(String txt)
{
    WebSerial.print(txt);
}

void eeprom_init()
{
    EEPROM.begin(512);
    if (!SPIFFS.begin())
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
    }
}

void web_init()
{

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, passwordAP);

    delay(1000);

    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html", String(), false, processor); });
    server.on("/vol", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    String inputMessage;
    // GET <ESP_IP>/vol?ring=<inputMessage>
    if (request->hasParam(RING_VOL)) {
      inputMessage = request->getParam(RING_VOL)->value();
      setRingVol(inputMessage);
      setRingGsm(inputMessage);
      WebSerial.print("Ring Volume: ");
      WebSerial.println(getRingVol());
      //analogWrite(output, sliderValue.toInt());
    }else if(request->hasParam(VOICE_VOL)){
      inputMessage = request->getParam(VOICE_VOL)->value();
      setVoiceVol(inputMessage);
      setVoiceGsm(inputMessage);
      WebSerial.print("Voice Volume: ");
      WebSerial.println(getVoiceVol());                                                                                                               
    }
    else {
      inputMessage = "No message sent";
    }
    //Serial.println(inputMessage);
    request->send(SPIFFS, "/index.html", String(), false, processor); });

    WebSerial.begin(&server);
    /* Attach Message Callback */
    WebSerial.msgCallback(recvMsg);
    server.begin();
    WebSerial.println("Roypro walkie-talkie start");
}

void load_default()
{
    char data;
    uint8_t len;
    uint8_t code;
    code = EEPROM.read(EE_NOMER_1);
    if (code == 0xAA)
    {
        // load nomer tersimpan
        len = EEPROM.read(EE_NOMER_1 + 1);
        kontrolNomer1 = "";
        for (uint8_t i = 0; i < len; i++)
        {
            data = EEPROM.read(EE_NOMER_1 + 2 + i);
            kontrolNomer1 += data;
        }

        WebSerial.print("Kontrol1: ");
        WebSerial.println(kontrolNomer1);
    }
    else
    {
        // kontrolNomer1 = "+6282265285223";

        WebSerial.print("Kontrol1 default: ");
        WebSerial.println(kontrolNomer1);
    }

    code = EEPROM.read(EE_NOMER_2);
    if (code == 0xAA)
    {
        // load nomer tersimpan
        len = EEPROM.read(EE_NOMER_2 + 1);
        kontrolNomer2 = "";
        for (uint8_t i = 0; i < len; i++)
        {
            data = EEPROM.read(EE_NOMER_2 + 2 + i);
            kontrolNomer2 += data;
        }

        WebSerial.print("Kontrol2: ");
        WebSerial.println(kontrolNomer2);
    }
    else
    {
        // kontrolNomer2 = "+6282265285223";

        WebSerial.print("Kontrol2 default: ");
        WebSerial.println(kontrolNomer2);
    }

    code = EEPROM.read(EE_NOMER_3);
    if (code == 0xAA)
    {
        // load nomer tersimpan
        len = EEPROM.read(EE_NOMER_3 + 1);
        kontrolNomer3 = "";
        for (uint8_t i = 0; i < len; i++)
        {
            data = EEPROM.read(EE_NOMER_3 + 2 + i);
            kontrolNomer3 += data;
        }

        WebSerial.print("Kontrol3: ");
        WebSerial.println(kontrolNomer3);
    }
    else
    {
        // kontrolNomer3 = "+6282265285223";

        WebSerial.print("Kontrol3 default: ");
        WebSerial.println(kontrolNomer3);
    }
}
