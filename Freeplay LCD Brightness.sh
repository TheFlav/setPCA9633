#!/usr/bin/env bash
sudo killall joy2key.py
/opt/retropie/supplementary/runcommand/joy2key.py /dev/input/js0 "0x44" "0x43" "0x43" "0x44" "0x20" "0x20" &
if [ -c "/dev/i2c-1" ]
then
	/home/pi/Freeplay/setPCA9633/setPCA9633interactive D C ' ' 5 | dialog --gauge "Freeplay LCD Brightness" 6 30 0
	/home/pi/Freeplay/setPCA9633/setPCA9633 -y 1 -a 0x62 -e 0 -v 1 > /home/pi/Freeplay/setPCA9633/fpbrightness.val
else
	dialog --msgbox "I2C has not been enabled yet. Go to RetroPie Menu, enter the Raspi Config, and then turn on I2C in the Devices/Interfaces section." 15 40 
fi
sudo killall joy2key.py
