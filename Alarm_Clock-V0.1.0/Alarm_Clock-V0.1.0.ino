#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <EEPROM.h>
#include <TimeLib.h>

char * ssid = "lockeland";
char * pass = "timmygraciesammy123";

char preset = 'y';

int speakerPin = 0;
int snooze_pin = 16;
int alarm_pin = 14;

float iter_count = 0.0f;
int alarm_run_count = 0;

unsigned int local_port = 2390;

IPAddress time_serverIP;
const char* ntp_server_name = "time.nist.gov";

const int NTP_PACKET_SIZE = 48;

const uint32_t seventyYears = 2208988800UL;

byte packetBuffer[NTP_PACKET_SIZE];

WiFiUDP udp;

void setup () 
{
  EEPROM.begin(512);
  EEPROM.get(0, preset);
  if(preset == 'n')
  {
    EEPROM.get(1, ssid);
    EEPROM.get(1+sizeof(*ssid), *pass);
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

  Serial.println("Opening UDP Port!");
  udp.begin(local_port);
  bool gotTime = false;
  while(!gotTime)
  {
    gotTime = getTime();
  }
  char h_buf[2];
  char m_buf[2];
  getDigits(hour(),h_buf);
  getDigits(minute(),m_buf);
  Serial.print("The time is: ");
  Serial.print(h_buf);
  Serial.print(":");
  Serial.println(m_buf);
  Serial.print("Time but wut: ");
  Serial.print(hour());
  Serial.print(":");
  Serial.println(minute());
}

void loop () 
{
  
  
}

void alarm_start()
{
  bool snooze_pressed = false;
  bool alarm_state = true;
  while(!snooze_pressed && alarm_state && iter_count < 30)
  {
    tone(speakerPin, 1000);
    delay(250);
    noTone(speakerPin);
    delay(250);
    iter_count = iter_count + 0.5f;
    snooze_pressed = digitalRead(snooze_pin);
    alarm_state = digitalRead(alarm_pin);
  }
  alarm_state = digitalRead(alarm_pin);
  if(iter_count >= 30 && alarm_run_count < 5 && !alarm_state)
  {
    alarm_run_count++;
    
    alarm_start();
  }
}

unsigned long sendNTPpacket(IPAddress& address)
{
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

void getDigits(uint32_t digits, char * buf) {
  char buf2[2];
 if(digits < 10)
 {
   buf[0] = '0';
 }
 else
 {
   itoa(digits/10, buf2, 2);
   buf[0] = buf2[0];
 }
 itoa(digits - ((digits/10) * 10),buf2, 2);
 buf[1] = buf2[0];
}

bool getTime()
{
  WiFi.hostByName(ntp_server_name, time_serverIP);
  sendNTPpacket(time_serverIP);
  delay(500);
  int cb = udp.parsePacket();
  if(!cb)
  {
    Serial.println("No pakcet yet.");
    return false;
  }
  else
  {
    udp.read(packetBuffer, NTP_PACKET_SIZE);
    uint32_t highWord = word(packetBuffer[40], packetBuffer[41]);
    uint32_t longWord = word(packetBuffer[42], packetBuffer[43]);
    uint32_t secs_since_1900 = highWord << 16 | longWord;
    long int secs_since_1970 = secs_since_1900 - seventyYears;
    setTime(secs_since_1970);
    return true;
  }
}
