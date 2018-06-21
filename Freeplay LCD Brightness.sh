#!/usr/bin/env bash
sudo killall joy2key.py
/opt/retropie/supplementary/runcommand/joy2key.py /dev/input/js0 "0x44" "0x43" "0x43" "0x44" "0x20" "0x20" &
if [ -c "/dev/i2c-1" ]
then
	if ! /home/pi/Freeplay/setPCA9633/setPCA9633 -y 1 -a 0x62 -e 0 -v 0;
	then
		dialog --title "Error" --msgbox "Freeplay backlight brightness controller not detected.  Are you sure you have one installed?" 15 40 
	else
		/home/pi/Freeplay/setPCA9633/setPCA9633interactive D C ' ' 5 | dialog --gauge "Freeplay LCD Brightness" 6 30 0
		/home/pi/Freeplay/setPCA9633/setPCA9633 -y 1 -a 0x62 -e 0 -v 1 > /home/pi/Freeplay/setPCA9633/fpbrightness.val
	fi
else
<<<<<<< HEAD
	dialog --msgbox "I2C has not been enabled yet. Go to RetroPie Menu, enter the Raspi Config, and then turn on I2C in the Devices/Interfaces section. This will only work if you have a controller installed." 15 40 
=======
	dialog --title "Error" --msgbox "I2C has not been enabled yet. Go to RetroPie Menu, enter the Raspi Config, and then turn on I2C in the Devices/Interfaces section." 15 40 
>>>>>>> a658faee71f32e4032a6ce49afc37371badffa05
fi
sudo killall joy2key.py
