#!/usr/bin/env bash
make
cp Freeplay\ LCD\ Brightness.sh /home/pi/RetroPie/retropiemenu/Freeplay\ LCD\ Brightness.sh
sudo sed -i 's|</gameList>|\t<game>\n\t\t<path>./Freeplay LCD Brightness.sh</path>\n\t\t<name>Freeplay Brightness Control</name>\n\t\t<desc>Control your screen brightness. Persists on reboot.</desc>\n\t\t<image>./icons/Brightness.png</image>\n\t\t<playcount>0</playcount>\n\t\t<lastplayed></lastplayed>\n\t</game>\n\t</gameList>|' /opt/retropie/configs/all/emulationstation/gamelists/retropie/gamelist.xml

sudo cp /boot/freeplaycfg.txt /home/pi/freeplaycfg.txt
sudo cp ./fpbrightness.val /home/pi/Freeplay/setPCA9633/fpbrightness.val
sudo sh -c 'printf "\n/home/pi/Freeplay/setPCA9633/setFreeplayBacklight.sh > /dev/null" >> /home/pi/freeplaycfg.txt'
sudo mv /boot/freeplaycfg.txt /boot/freeplaycfg.bak
sudo mv /home/pi/freeplaycfg.txt /boot/freeplaycfg.txt
cp Brightness.png /home/pi/RetroPie/retropiemenu/icons/Brightness.png

sudo sh -c 'echo "[pi3]\ndtparam=i2c1_baudrate=400000 #makes a big speed difference\ndtoverlay=i2c1-bcm2708,sda1_pin=44,scl1_pin=45,pin_func=6\n[all]" >> /boot/config.txt'
