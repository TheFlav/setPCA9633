#!/usr/bin/env bash
make
mv "Freeplay LCD Brightness.sh" /home/pi/RetroPie/retropiemenu/"Freeplay LCD Brightness.sh"
sudo cp /boot/freeplaycfg.txt /home/pi/freeplaycfg.txt
echo "/home/pi/Freeplay/setPCA9633/setFreeplayBacklight.sh > /dev/null" > /home/pi/freeplaycfg.txt
sudo mv /boot/freeplaycft.txt /boot/freeplaycfg.bak
sudo mv /home/pi/freeplaycfg.txt /boot/freeplaycfg.txt
mv Brightness.png /home/pi/RetroPie/retropiemenu/Brightness.png

sudo sed -i 's|</gameList>|\t<game>\n\t\t<path>./"Freeplay LCD Brightness.sh"</path>\n\t\t<name>FreePlay Brightness Control</name>\n\t\t<desc>Control your screen brightness. Persists on reboot.</desc>\n\t\t<image>./icons/Brightness.png</image>\n\t\t<playcount>0</playcount>\n\t\t<lastplayed></lastplayed>\n\t</game>\n\t</gameList>' /opt/retropie/configs/all/emulationstation/gamelists/retropie/gamelist.xml
