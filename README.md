# ESP8266-Alarm-Clock
A humiliating alarm clock based ont he ESP8266 board

A current WIP. Based on the ESP8266 board, this clock will send out a humiliatign message onto twitter or facebook, every tiemt ehh snooze button is pressed. Alarm setting will be doen through the web interface, along with what facebook/tiwtter to tag in the post. Each query will be sent to a remote server to queue the amoutn of times a message is sent, to keep within the api limtis of the platforms.

The web interface will be activated by moving a switch, and restarting the clock. It will boot into wifi access point mode, with a  default name and password that cannot be changed. When ready to revert back to clock mode, the swithc will eb turned off, and the clock restarted.

The clock will feature a RTC interface, and a 4 digit 7-segment dispaly with digit sizes of 1.2 inches.
The case will be 3D Printed. Snooze button ontop, and switches on the back.

The alarm will have its own swithc to enable it, or turn it off and avoid a humiliating callout

Alarm will be a single piezo buzzer buzzing at 500Hz at a rate of 2Hz (500Hz tones, twice a second) for one minute.
After one minute, if the alarm is not off and snooze not pressed, it will be treated as snooze. Snooze will bre repated up to five tiems until shutting off for the day.

Time will be in 24-Hour format

ESP8266 board will only facilitate the updating of the time, and alarm clock function. The RTC will generate the 2Hz signal to pulse the 500Hz tone on and off

When the boar ddetects the alamr time has been activated, it will generate the 500Hz tone

im wiritng this at 2AM gimme a break im tired

As always al my work is open source. If soemoen does it better or cheaper, good for them.

TODO:
Add clock dispaly support
Add RTC support
Make a basic alarm system with the ability to change it in the EEPROM easily (fucntions?)
Add web interface
