#include <Arduino.h>
#include <webHandle.h>

#define BTN_PIN 2
#define SPEAKER_PIN 0

#define RING_COUNT 2

// #define CALL_PIN 3
// #define SPEAKER_PIN 1

String kontrolNomer1 = "+6282265285223";
String kontrolNomer2 = "+6282113555000";
String kontrolNomer3 = "+6282265285223";

String password = "9876";
String sinyal = "99";
String batt = "0";
String vol = "0";

String serBUff = "";
bool ringSts = false;
int ringCount = 0;
bool callSts = false;

uint8_t statusCount = 0;

uint8_t ledTime = 5;
bool ledStatus = false;
bool led_on = false; // led off
int stsCount = 0;
bool web_sts = false;

unsigned int statusUpdate = 1000;
unsigned long timeUpdate = millis();

uint8_t speakerVol = 50;
uint8_t lastSpeakerVol = 50;

String ringValue = "0";
String voiceValue = "0";

void _readSerial(uint32_t timeout);
bool deleteAllRead();
bool initCall();
String read(unsigned int index, bool markRead);
void speakerOFF();
void getRingGsm();
void getVoiceGsm(); 

void io_init()
{
    pinMode(SPEAKER_PIN, OUTPUT);
    pinMode(BTN_PIN, INPUT_PULLUP);
    speakerOFF();
}

void gsm_init()
{

    delay(10000);

    Serial.println(F("AT+CMGF=1"));
    delay(1000);
    Serial.println(F("AT+CNMI=1,2,0,0,0"));
    delay(1000);
    Serial.println(F("ATE0"));
    delay(1000);
    deleteAllRead();
    delay(1000);
    initCall();
    delay(1000);
    getRingGsm();
    delay(1000);
    getVoiceGsm();
}

void kirimAt(String at)
{
    at += "\r";
    Serial.print(at);
}



void setRingGsm(String vol)
{
    String cmd = "AT+CRSL=";
    cmd += vol;
    kirimAt(cmd);
}

void setVoiceGsm(String vol)
{
    String cmd = "AT+CLVL=";
    cmd += vol;
    kirimAt(cmd);
}

void setRingVol(String r)
{
    ringValue = r;
}

void setVoiceVol(String v)
{
    voiceValue = v;
}

void getRingGsm()
{
    // AT+CRSL?
    Serial.print(F("AT+CRSL?\r"));

    _readSerial(5000);

    int startMsg = serBUff.indexOf(F("+CRSL:"));

    if (startMsg != -1)
    {
        int endMsg = serBUff.indexOf("\\r\\l");
        ringValue = serBUff.substring(startMsg + 7, endMsg);
        sendWebSerial("Ring volume ");
        sendWebSerial(ringValue);
    }
}

void getVoiceGsm()
{
    // AT+CRSL?
    Serial.print(F("AT+CLVL?\r"));

    _readSerial(5000);

    int startMsg = serBUff.indexOf(F("+CLVL:"));

    if (startMsg != -1)
    {
        int endMsg = serBUff.indexOf("\\r\\l");
        voiceValue = serBUff.substring(startMsg + 7, endMsg);
        sendWebSerial("Speaker volume ");
        sendWebSerial(voiceValue);
    }
}

String getRingVol()
{
    return ringValue;
}

String getVoiceVol()
{
    return voiceValue;
}

bool callAnswer()
{
    Serial.print(F("ATA\r"));

    _readSerial(5000);

    if (serBUff.indexOf(F("OK")) != -1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void callReject(){
    Serial.print(F("ATH\r"));
}

void _readSerial(uint32_t timeout)
{
    serBUff = "";
    uint64_t timeOld = millis();
    // Bir süre bekle...
    while (!Serial.available() && !(millis() > timeOld + timeout))
    {
        ;
    }
    // beklemeden çıkınca ya da süre dolunca varsa seriali oku, yoksa çık git...
    if (Serial.available())
    {
        serBUff = Serial.readString();
    }
}

bool setTextMode(bool textModeON)
{
    if (textModeON == true)
    {
        Serial.print(F("AT+CMGF=1\r"));
    }
    else
    {
        Serial.print(F("AT+CMGF=0\r"));
    }
    _readSerial(5000);

    if (serBUff.indexOf("OK") != -1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool setPreferredSMSStorage(char *mem1, char *mem2, char *mem3)
{
    Serial.print(F("AT+CPMS=\""));
    Serial.print(mem1);
    Serial.print(F("\",\""));
    Serial.print(mem2);
    Serial.print(F("\",\""));
    Serial.print(mem3);
    Serial.print(F("\"\r"));

    _readSerial(5000);

    if (serBUff.indexOf("OK") != -1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool setNewMessageIndication()
{
    Serial.print(F("AT+CNMI=2,1\r"));
    _readSerial(5000);

    if (serBUff.indexOf("OK") != -1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool setCharset(char *charset)
{
    Serial.print(F("AT+CSCS=\""));
    Serial.print(charset);
    Serial.print(F("\"\r"));
    _readSerial(5000);

    if (serBUff.indexOf("OK") != -1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool initSMS()
{
    if (setTextMode(true))
    {
        if (setPreferredSMSStorage("ME", "ME", "ME"))
        {
            if (setNewMessageIndication())
            {
                if (setCharset("IRA"))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool deleteAllRead()
{
    Serial.print(F("AT+CMGD=1,1\r"));

    _readSerial(5000);

    if (serBUff.indexOf("OK") != -1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// call
bool setCOLP(bool active)
{
    int durum = active == true ? 1 : 0;
    Serial.print(F("AT+COLP="));
    Serial.print(durum);
    Serial.print("\r");

    _readSerial(5000);

    if (serBUff.indexOf(F("OK")) == -1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool showCurrentCall(bool active)
{
    int durum = active == true ? 1 : 0;
    Serial.print(F("AT+CLCC="));
    Serial.print(durum);
    Serial.print("\r");

    _readSerial(5000);

    if (serBUff.indexOf(F("OK")) == -1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool setCLIP(bool active)
{
    int durum = active == true ? 1 : 0;
    Serial.print(F("AT+CLIP="));
    Serial.print(durum);
    Serial.print("\r");

    _readSerial(5000);

    if (serBUff.indexOf(F("OK")) == -1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool setCLIR(bool active)
{
    int durum = active == true ? 1 : 0;
    Serial.print(F("AT+CLIR="));
    Serial.print(durum);
    Serial.print("\r");

    _readSerial(5000);

    if (serBUff.indexOf(F("OK")) == -1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool initCall()
{
    if (setCOLP(true))
    {
        if (showCurrentCall(true))
        {
            if (setCLIP(false))
            {
                if (setCLIR(false))
                {
                    if (showCurrentCall(true))
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

void speakerON()
{
    digitalWrite(SPEAKER_PIN, LOW);
}

void speakerOFF()
{
    digitalWrite(SPEAKER_PIN, HIGH);
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++)
    {
        if (data.charAt(i) == separator || i == maxIndex)
        {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }

    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

unsigned int indexFromSerial(String serialRaw)
{
    if (serialRaw.indexOf("+CMTI:") != -1)
    {
        String numara = serialRaw.substring(serialRaw.indexOf("\",") + 2);
        numara.trim();
        int no = numara.toInt();

        return no;
    }
    else
    {
        return -1;
    }
}

String readFromSerial(String serialRaw)
{
    if (serialRaw.indexOf("+CMTI:") != -1)
    {
        String numara = serialRaw.substring(serialRaw.indexOf("\",") + 2);
        numara.trim();
        int no = numara.toInt();

        return read(no, true);
    }
    else
    {
        return "RAW_DATA_NOT_READ";
    }
}

String read(unsigned int index, bool markRead)
{
    Serial.print("AT+CMGR=");
    Serial.print(index);
    Serial.print(",");
    if (markRead == true)
    {
        Serial.print("0");
    }
    else
    {
        Serial.print("1");
    }
    Serial.print("\r");

    _readSerial(30000);

    // return _buffer;

    String durum = "INDEX_NO_ERROR";
    // String pesan="";

    if (serBUff.indexOf("+CMGR:") != -1)
    {

        String klasor, okundumu, telno, zaman, mesaj;

        klasor = "UNKNOWN";
        okundumu = "UNKNOWN";

        if (serBUff.indexOf("REC UNREAD") != -1)
        {
            klasor = "INCOMING";
            okundumu = "UNREAD";
        }
        if (serBUff.indexOf("REC READ") != -1)
        {
            klasor = "INCOMING";
            okundumu = "READ";
        }
        if (serBUff.indexOf("STO UNSENT") != -1)
        {
            klasor = "OUTGOING";
            okundumu = "UNSENT";
        }
        if (serBUff.indexOf("STO SENT") != -1)
        {
            klasor = "OUTGOING";
            okundumu = "SENT";
        }

        String telno_bol1 = serBUff.substring(serBUff.indexOf("\",\"") + 3);
        telno = telno_bol1.substring(0, telno_bol1.indexOf("\",\"")); // telefon numarası tamam

        String tarih_bol = telno_bol1.substring(telno_bol1.lastIndexOf("\",\"") + 3);

        zaman = tarih_bol.substring(0, tarih_bol.indexOf("\"")); // zamanı da aldık. Bir tek mesaj kaldı!

        mesaj = tarih_bol.substring(tarih_bol.indexOf("\"") + 1, tarih_bol.lastIndexOf("OK"));

        mesaj.trim();

        // Little Fix for incoming messaged from iPhone
        String messageHex = "";
        for (int i = 0; i < mesaj.length(); i++)
        {
            messageHex += String(mesaj[i], HEX);
        }

        if (messageHex.indexOf("ffa5ffa4ffa3ffa3") != -1)
        {
            mesaj = mesaj.substring(4);
        }

        durum = "FOLDER:";
        durum += klasor;
        durum += "|STATUS:";
        durum += okundumu;
        durum += "|PHONENO:";
        durum += telno;
        durum += "|DATETIME:";
        durum += zaman;
        durum += "|MESSAGE:";
        durum += mesaj;
        // pesan = mesaj;
    }

    return durum;
}

String getSenderNo(unsigned int index)
{
    String message = read(index, false);

    if (message.indexOf(F("PHONENO:")) == -1)
    {
        return "NO MESSAGE";
    }
    else
    {
        String no = message.substring(message.indexOf("PHONENO:") + 8, message.indexOf("|DATETIME:"));
        no.trim();
        return no;
    }
}

String getPesan(String msg)
{
    // Verilen indexten mesajı gönderen kişiyi bulur. +

    // String message = read(index, false);

    if (msg.indexOf(F("|MESSAGE:")) == -1)
    {
        return "NO MESSAGE";
    }
    else
    {
        String pesan = msg.substring(msg.indexOf("|MESSAGE:") + 9, msg.length());
        // no.trim();
        return pesan;
    }
}

void cekSms(String msg, String sender)
{
    String resetNomer = "00000000000";
    String out = "";
    if (msg.indexOf("Reg1 9876") != -1)
    {
        simpan_kontrolNomer(1, sender);
        kontrolNomer1 = sender;
        out = "Simpan Sender di M1: ";
        out += sender;
        sendWebSerial(out);
    }
    else if (msg.indexOf("Reg2 9876") != -1)
    {
        simpan_kontrolNomer(2, sender);
        kontrolNomer2 = sender;
        out = "Simpan Sender di M2: ";
        out += sender;
        sendWebSerial(out);
    }
    else if (msg.indexOf("Reg3 9876") != -1)
    {
        simpan_kontrolNomer(3, sender);
        kontrolNomer3 = sender;
        out = "Simpan Sender di M3: ";
        out += sender;
        sendWebSerial(out);
    }
    else if (msg.indexOf("Del1 9876") != -1)
    {
        simpan_kontrolNomer(1, resetNomer);
        kontrolNomer1 = resetNomer;
        sendWebSerial("Reset nomer 1");
    }
    else if (msg.indexOf("Del2 9876") != -1)
    {
        simpan_kontrolNomer(2, resetNomer);
        kontrolNomer2 = resetNomer;
        sendWebSerial("Reset nomer 2");
    }
    else if (msg.indexOf("Del3 9876") != -1)
    {
        simpan_kontrolNomer(3, resetNomer);
        kontrolNomer3 = resetNomer;
        sendWebSerial("Reset nomer 3");
    }
}
/*
void ledUpdate()
{
  // update dalam tiap 50 ms
  if (led_on)
  {
    if (++ledTime > led_dur)
    {
      ledTime = 0;
      led_on = false;

      int sn = sinyal.toInt();
      int bt = batt.toInt();
      // led off
      if (sn == 99)
      {
        led_dur = 60;
      }
      else
      {
        ledOFF();
        if (sn <= 10)
        {
          // blue_on();
          led_dur = 30;
        }
        else if ((sn > 10) && (sn < 24))
        {
          // blue_on();
          led_dur = 60;
          // if(bt < 20){
          //   red_on();
          // }
        }
        else if ((sn >= 24) && (sn < 32))
        {
          // blue_on();
          led_dur = 90;
          // if(bt < 20){
          //   red_on();
          // }
        }
      }
    }
  }
  else
  {
    if (++ledTime > led_dur)
    {
      ledTime = 0;
      led_on = true;
      // led on;
      int sn = sinyal.toInt();
      int bt = batt.toInt();
      if (sn == 99)
      {
        // no network
        ledON();
      }
      else if (sn <= 10)
      {
        ledON();
        led_dur = 2;
      }
      else if ((sn > 10) && (sn < 24))
      {
        ledON();
        led_dur = 2;
        // if(bt < 20){
        //   red_on();
        // }
      }
      else if ((sn >= 24) && (sn < 32))
      {
        ledON();
        led_dur = 2;
        // if(bt < 20){
        //   red_on();
        // }
      }
    }
  }
}
*/
void kirim_nomerKontrol()
{
    String kontrolNomer = kontrolNomer1;
    kontrolNomer += "\n";
    kontrolNomer += kontrolNomer2;
    kontrolNomer += "\n";
    kontrolNomer += kontrolNomer3;
    kontrolNomer += "\n";

    sendWebSerial(kontrolNomer);
}

void sim800_loop()
{

    if ((millis() - timeUpdate) > 50)
    {
        timeUpdate = millis();

        // ledUpdate();

        if (++statusCount > 200)
        {
            statusCount = 0;
            if (++stsCount > 1)
            {
                stsCount = 0;
            }
            if (stsCount == 0)
            {
                Serial.print(F("AT+CSQ\r"));
            }
            else if (stsCount == 1)
            {
                Serial.print(F("AT+CBC\r"));
            }
            else if (stsCount == 2)
            {
                Serial.print(F("AT+CLVL?\r"));
            }
        }
    }

    if (Serial.available())
    {
        serBUff = "";
        serBUff = Serial.readString();
        sendWebSerial(serBUff);

        // This example for how you catch incoming calls.
        if (serBUff.indexOf("+CLCC:") != -1)
        {
            String CLCCvalue = serBUff.substring(serBUff.indexOf(F("+CLCC:"))); // on answer call, it give +COLP: xxxx OK and +CLCC: xxx. So we must split it.
            String callStatus = CLCCvalue.substring(CLCCvalue.indexOf(F("+CLCC:")) + 11, CLCCvalue.indexOf(F("+CLCC:")) + 12);
            String callNomer = CLCCvalue.substring(CLCCvalue.indexOf(F(",\"")) + 2, CLCCvalue.indexOf(F("\",")));

// String callNomer = call.readCurrentCall(serBUff);
// Serial.println(callNomer);

            if (!callSts)
            {
                if (!ringSts)
                {
                    if ((kontrolNomer1 == callNomer) || (kontrolNomer2 == callNomer) || (kontrolNomer3 == callNomer))
                    {
                        ringSts = true;
                        ringCount = 0;
                        speakerON();
                    }else{
                        ringSts = false;
                        ringCount = 0;
                        callReject();
                    }
                }
            }

            if (callStatus == "6")
            {
                if (serBUff.indexOf(F("BUSY")) != -1)
                {
                    // durum = "STATUS:BUSY"; // kullanıcı meşgul bitti
                }
                else
                {
                    // durum = "STATUS:CALLEND"; // görüşme bitti
                    speakerOFF();
                    ringSts = false;
                    ringCount = 0;
                    callSts = false;
                }
            }
        }
        else if (serBUff.indexOf("+CMT:") != -1)
        {

            // int indexno = indexFromSerial(serBUff);
            String senderNomer = serBUff.substring(serBUff.indexOf("+CMT:") + 7, serBUff.indexOf("\",\""));
            String smsContent = serBUff.substring(serBUff.indexOf("\"\r\n") + 2, serBUff.indexOf("#\r\n"));
            // cekSms(smsContent, senderNomer);

            String smsC = "Terima pesan Idx: ";
            // smsC += serBUff;
            // smsC += String(indexno);
            smsC += " Sender: ";
            smsC += senderNomer;
            smsC += " Pesan: ";
            smsC += smsContent;
            cekSms(smsContent,senderNomer);

            /*
                if (smsContent == "Cek nomer")
                {
                  kirim_nomerKontrol();
                }

                const char *sender_nm = senderNomer.c_str();
                size_t length = strlen(sender_nm);

                char *nm = new char[length + 1];
                strcpy(nm, sender_nm);

                if (senderNomer == kontrolNomer1)
                {
                  // response
                  // sms.send(nm,"Pesan diterima");
                }
                deleteAllRead();
                delete[];
                */
        }
        else if ((serBUff.indexOf(F("+CSQ:"))) != -1)
        {

            sinyal = serBUff.substring(serBUff.indexOf(F("+CSQ: ")) + 6, serBUff.indexOf(F(",")));
        }
        else if ((serBUff.indexOf(F("+CBC:"))) != -1)
        {

            batt = getValue(serBUff, ',', 1);
        }
        else if ((serBUff.indexOf(F("+CLVL:"))) != -1)
        {
            vol = serBUff.substring(serBUff.indexOf(F("+CLVL: ")) + 7, (serBUff.indexOf(F("OK")) - 4));
            vol.trim();
            speakerVol = vol.toInt();
        }
        else if ((serBUff.indexOf(F("RING"))) != -1)
        {
            if (ringSts)
            {
                if (++ringCount > RING_COUNT)
                {
                    ringCount = 0;
                    callSts = true;
                    callAnswer();
                }
            }
        }
    }
}
