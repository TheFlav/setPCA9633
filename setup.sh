#!/usr/bin/env bash
make
cp Freeplay\ LCD\ Brightness.sh /home/pi/RetroPie/retropiemenu/Freeplay\ LCD\ Brightness.sh
if  grep -q "Freeplay LCD Brightness" /opt/retropie/configs/all/emulationstation/gamelists/retropie/gamelist.xml ; then
	echo "Brightness already in RetroPie Menu"
else
	sudo sed -i 's|</gameList>|\t<game>\n\t\t<path>./Freeplay LCD Brightness.sh</path>\n\t\t<name>Freeplay Brightness Control</name>\n\t\t<desc>Control your screen brightness. Persists on reboot.</desc>\n\t\t<image>./icons/Brightness.png</image>\n\t\t<playcount>0</playcount>\n\t\t<lastplayed></lastplayed>\n\t</game>\n\t</gameList>|' /opt/retropie/configs/all/emulationstation/gamelists/retropie/gamelist.xml
fi

if  grep -q "setFreeplayBacklight.sh" /etc/rc.local ; then
	echo "Backlight already setup"
else
	sudo sed -i 's|exit 0|/home/pi/Freeplay/setPCA9633/setFreeplayBacklight.sh > /dev/null\n\nexit 0|' /etc/rc.local
fi

cp Brightness.png /home/pi/RetroPie/retropiemenu/icons/Brightness.png

sudo sh -c 'printf "[pi3]\ndtparam=i2c1_baudrate=400000 #makes a big speed difference\ndtoverlay=i2c1-bcm2708,sda1_pin=44,scl1_pin=45,pin_func=6,combine=off\n[all]" >> /boot/config.txt'
