#!/usr/bin/env bash
make
cp Freeplay\ LCD\ Brightness.sh /home/pi/RetroPie/retropiemenu/Freeplay\ LCD\ Brightness.sh
sudo sed -i 's|</gameList>|\t<game>\n\t\t<path>./Freeplay LCD Brightness.sh</path>\n\t\t<name>FreePlay Brightness Control</name>\n\t\t<desc>Control your screen brightness. Persists on reboot.</desc>\n\t\t<image>/home/pi/Freeplay/setPCA9633/Brightness.png</image>\n\t\t<playcount>0</playcount>\n\t\t<lastplayed></lastplayed>\n\t</game>\n\t</gameList>|' /opt/retropie/configs/all/emulationstation/gamelists/retropie/gamelist.xml

cp /boot/freeplaycfg.txt /home/pi/freeplaycfg.txt
sudo sh -c 'echo "/home/pi/Freeplay/setPCA9633/setFreeplayBacklight.sh > /dev/null" >> /home/pi/freeplaycfg.txt'
mv /boot/freeplaycfg.txt /boot/freeplaycfg.bak
mv /home/pi/freeplaycfg.txt /boot/freeplaycfg.txt
cp Brightness.png /home/pi/RetroPie/retropiemenu/icons/Brightness.png

sudo sh -c 'echo "[pi3]\ndtparam=i2c1_baudrate=400000 #makes a big speed difference\ndtoverlay=i2c1-bcm2708,sda1_pin=44,scl1_pin=45,pin_func=6\n" >> /boot/config.txt'
