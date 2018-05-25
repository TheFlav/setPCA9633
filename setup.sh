#!/usr/bin/env bash
make
mv "Freeplay LCD Brightness.sh" /home/pi/RetroPie/retropiemenu/"Freeplay LCD Brightness.sh"
sudo cp /boot/freeplaycfg.txt /home/pi/freeplaycfg.txt
sudo chmod 777 /home/pi/freeplaycfg.txt
echo "/home/pi/Freeplay/setPCA9633/setFreeplayBacklight.sh >> /dev/null" >> /home/pi/freeplaycfg.txt
sudo mv /boot/freeplaycfg.txt /boot/freeplaycfg.bak
sudo mv /home/pi/freeplaycfg.txt /boot/freeplaycfg.txt
mv Brightness.png /home/pi/RetroPie/retropiemenu/icons/Brightness.png

sudo chmod 777 /boot/config.txt
sudo sed 's|\[pi3\]|\[pi3\]\ndtparam=i2c1_baudrate=400000 #makes a big speed difference\ndtoverlay=i2c-1bcm2708,sda1_pin=45,pin_func=6\n[all]|' /boot/config.txt
sudo sed -i 's|</gameList>|\t<game>\n\t\t<path>./"Freeplay LCD Brightness.sh"</path>\n\t\t<name>FreePlay Brightness Control</name>\n\t\t<desc>Control your screen brightness. Persists on reboot.</desc>\n\t\t<image>./icons/Brightness.png</image>\n\t\t<playcount>0</playcount>\n\t\t<lastplayed></lastplayed>\n\t</game>\n\t</gameList>|' /opt/retropie/configs/all/emulationstation/gamelists/retropie/gamelist.xml
