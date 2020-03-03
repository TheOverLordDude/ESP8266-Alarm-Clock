#include <Timezone.h>   // https://github.com/JChristensen/Timezone

// US Eastern Time Zone (New York, Detroit)
TimeChangeRule usEdt = {"EDT", Second, Sun, Mar, 2, -240};    // UTC - 4 hours
TimeChangeRule usEst = {"EST", First, Sun, Nov, 2, -300};     // UTC - 5 hours
TimeChangeRule usPST = {"PST", Second, Sun, Mar, 2, (-1 * 7 * 60)};
TimeChangeRule usPDT = {"PST", Second, Sun, Mar, 2, (-1 * 8 * 60)};
Timezone usEastern(usEdt, usEst);
Timezone usPacific(usPST, usPDT);

void setup()
{
    Serial.begin(115200);
    usEastern.writeRules(512);    // write rules to EEPROM address 100
    Serial.println("Writing complete.");
}

void loop()
{

}
