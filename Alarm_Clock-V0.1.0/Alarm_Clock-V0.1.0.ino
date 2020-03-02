#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <EEPROM.h>
#include <Time.h>

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
}

void loop () 
{
  WiFi.hostByName(ntp_server_name, time_serverIP);

  sendNTPpacket(time_serverIP);
  delay(1000);
  int cb = udp.parsePacket();
  if(!cb)
  {
    Serial.println("No pakcet yet.");
  }
  else
  {
    udp.read(packetBuffer, NTP_PACKET_SIZE);
    long int highWord = word(packetBuffer[40], packetBuffer[41]);
    long int longWord = word(packetBuffer[42], packetBuffer[43]);
    long int secs_since_1900 = highWord << 16 | longWord;
    Serial.print("Secodns since Jan 1st, 1990: ");
    Serial.println(secs_since_1900);
    time.setTime(secs_since_1900);
    Serial.print(hour());
    printDigits(minute());
    delay(500);
  }
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
    
    alarm_start()
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

void printDigits(int digits) {
 // utility function for digital clock display: prints preceding colon and leading 0
 Serial.print(":");
 if (digits < 10)
 Serial.print('0');
 Serial.print(digits);
}
