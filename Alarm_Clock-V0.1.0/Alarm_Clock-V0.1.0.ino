#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <EEPROM.h>
#include <ezTime.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#include <RtcDS1307.h>

RtcDS1307<TwoWire> rtc(Wire);

Adafruit_7segment _7seg = Adafruit_7segment();

char * ssid = "lockeland";
char * pass = "timmygraciesammy123";

char preset = 'y';
char alarmSet = 'y';

int speakerPin = 0;
int snooze_pin = 16;
int alarm_pin = 14;

int alarmCount = 0;

unsigned long deltaTime = millis();

int secondsCount = 0;

bool alarmSleep = false;
bool alarmOn = false;
bool alarmSnoozed = false;

uint8_t lastSecond = 0;

RtcDateTime compiled(0,0,0,0,0,0);

RtcDateTime alarmTime(0,0,0,0,0,0);

void setup () 
{
  EEPROM.begin(512);
  EEPROM.get(0, preset);
  EEPROM.get(200, alarmSet);
  if(preset == 'n')
  {
    EEPROM.get(1, ssid);
    EEPROM.get(1+sizeof(*ssid), *pass);
  }
  if(alarmSet == 'n')
  {
    EEPROM.get(201, alarmTime);
  }
  pinMode (speakerPin, OUTPUT);
  pinMode(alarm_pin, INPUT);
  pinMode(snooze_pin, INPUT);

  Serial.begin(115200);
  Serial.println("\nBeginning the clock interface! Starting network...");

  WiFi.begin(ssid, pass);
  Serial.print("Connecting..");
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(250);
  }
  Serial.println("");

  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  deltaTime = millis();
  Serial.println("Waiting for sync...");
  waitForSync();
  Serial.println("Syncing.");
  Timezone usPST;
  usPST.setLocation("America/Los_Angeles");
  usPST.setDefault();
  Serial.println("Timezoen set");
  int h_buf[2];
  int m_buf[2];
  getDigits(hour(),h_buf);
  getDigits(minute(),m_buf);
  Serial.print("The time is: ");
  Serial.print(h_buf[0]);Serial.print(h_buf[1]);
  Serial.print(":");
  Serial.print(m_buf[0]);Serial.println(m_buf[1]);
  Serial.print("Time but wut: ");
  Serial.print(hour());
  Serial.print(":");
  Serial.println(minute());
  _7seg.begin(0x70);
  attachInterrupt(digitalPinToInterrupt(15), snoozeButtonPressed, RISING);
  attachInterrupt(digitalPinToInterrupt(13), sleepButtonPressed, RISING);

  _7seg.clear();
   _7seg.drawColon(true);
   _7seg.writeDisplay();

  delay(1000);
  rtc.Begin();
  compiled = RtcDateTime(year(), month(), day(), hour(), minute(), second());
  compiled += uint32_t((deltaTime - millis())/1000);
  rtc.SetDateTime(compiled);
  deltaTime = millis();
  lastSecond = compiled.Second();
}



void loop () 
{
  if(millis() >= (30*24*60*60*1000))
  {
    ESP.restart();
  }
  bool timeCheck = false;
  if (lastSecond < compiled.Second())
  {
    timeCheck = true;
  }
  alarmOn = digitalRead(12);
  if(timeCheck == true)
  {
    compiled = rtc.GetDateTime();
    if(alarmOn)
    {
      if(compiled.Hour() == alarmTime.Hour() && compiled.Minute() == alarmTime.Minute())
      {
        alarm_start();
      }
      if(compiled.Hour() == alarmTime.Hour() && compiled.Minute()-alarmTime.Minute() >= (5 * (alarmCount + 1)) && (alarmSnoozed) && alarmCount < 3)
      {
        alarmCount++;
        alarmSnoozed = false;
        alarm_start();
      }
      else if(compiled.Hour() == alarmTime.Hour() && compiled.Minute()-alarmTime.Minute() >= (5 * (alarmCount)) && (!alarmSleep))
      {
        alarm_snoozed();
      }
    }
  }
  lastSecond = compiled.Second();
  deltaTime = millis() - deltaTime;
  delay(1000 - deltaTime);
  deltaTime = millis();
  compiled += 1;
}

void alarm_start()
{
  tone(2, 2);
  rtc.SetSquareWavePin(DS1307SquareWaveOut_4kHz);
}

void alarm_stop()
{
  noTone(2);
  rtc.SetSquareWavePin(DS1307SquareWaveOut_Low);
}

void alarm_snoozed()
{
  alarm_stop();
  alarmSnoozed = true;
  //send query to server
}

void updateDisplay()
{
  int h_buf[2];
  int m_buf[2];
  getDigits(compiled.Hour(),h_buf);
  getDigits(compiled.Minute(),m_buf);
  _7seg.print(h_buf[0], DEC);
  _7seg.print(h_buf[1], DEC);
  _7seg.print(m_buf[0], DEC);
  _7seg.print(m_buf[1], DEC);
  _7seg.writeDisplay();
}

ICACHE_RAM_ATTR void snoozeButtonPressed()
{
  alarm_stop();
  alarmSnoozed = true;
}

ICACHE_RAM_ATTR void sleepButtonPressed()
{
  alarmSleep = true;
  alarm_stop();
}

void getDigits(uint32_t digits, int * buf) {
 if(digits < 10)
 {
   buf[0] = 0;
 }
 else
 {
   buf[0] = digits/10;
 }
 buf[1] = digits - ((digits/10) * 10);
}
