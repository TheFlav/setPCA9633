
#!/bin/sh
var1="$1"
if [ "$1" == "ON" ] || [ "$1" == "on" ]; then
    /home/pi/Freeplay/setPCA9633/setPCA9633 -y 1 -a 0x62 -d ON -w WAKE -i YES -l ON
    /home/pi/Freeplay/setPCA9633/setPCA9633 -y 1 -a 0x62 -w SLEEP
elif [ "$1" == "OFF" ] || [ "$1" == "off" ]; then
    /home/pi/Freeplay/setPCA9633/setPCA9633 -y 1 -a 0x62 -d ON -w WAKE -i YES -l OFF
    /home/pi/Freeplay/setPCA9633/setPCA9633 -y 1 -a 0x62 -w SLEEP
else
    /home/pi/Freeplay/setPCA9633/setPCA9633 -y 1 -a 0x62 -d ON -w WAKE -i YES -l PWM -p "$var1"
fi
