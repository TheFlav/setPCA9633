#!/bin/bash
var1="$1"

if [ -z "$var1" ]; then
  fileval=`cat /home/pi/Freeplay/setPCA9633/fpbrightness.val`
  if [ -z "$fileval" ]; then
    var1="ON"
  else
    var1=$( printf "%x" $fileval )
  fi
  #echo "$var1"
fi

if [ "$var1" == "ON" ] || [ "$var1" == "on" ] || [ "$var1" == "255" ]; then
    /home/pi/Freeplay/setPCA9633/setPCA9633 -y 1 -a 0x62 -d ON -w WAKE -i YES -l ON
    /home/pi/Freeplay/setPCA9633/setPCA9633 -y 1 -a 0x62 -w SLEEP
elif [ "$var1" == "OFF" ] || [ "$var1" == "off" ] || [ "$var1" == "0" ]; then
    /home/pi/Freeplay/setPCA9633/setPCA9633 -y 1 -a 0x62 -d ON -w WAKE -i YES -l OFF
    /home/pi/Freeplay/setPCA9633/setPCA9633 -y 1 -a 0x62 -w SLEEP
else
    /home/pi/Freeplay/setPCA9633/setPCA9633 -y 1 -a 0x62 -d ON -w WAKE -i YES -l PWM -p "$var1"
fi
