#!/usr/bin/env bash

dialog --title "Freeplay PWM Utilities (Using PCA9633)" \
	--yesno "Would you like to also install PWM fan control in addition to brightness control?\nThis requires you follow the guide at <URL>" 0 0
clear
RESP=$?

INSTALL_DIR=/home/pi/Freeplay/$(ls /home/pi/Freeplay | grep -i setpca9633)
cd $INSTALL_DIR
case $RESP in
	0) sudo cp $INSTALL_DIR/PWMFan.service /lib/systemd/system/PWMFan.service; sudo systemctl enable PWMFan.service; sudo systemctl start PWMFan.service;;
	1) ;;
	255) echo "Exiting installation"; exit 1;;
esac

make setPCA9633
make setPCA9633interactive
cp $INSTALL_DIR/Freeplay\ LCD\ Brightness.sh /home/pi/RetroPie/retropiemenu/Freeplay\ LCD\ Brightness.sh

if  grep -q "Freeplay LCD Brightness" /opt/retropie/configs/all/emulationstation/gamelists/retropie/gamelist.xml ; then
	echo "Brightness Control has already been added to RetroPie Menu"
else
	sudo sed -i 's|</gameList>|\t<game>\n\t\t<path>./Freeplay LCD Brightness.sh</path>\n\t\t<name>Freeplay Brightness Control</name>\n\t\t<desc>Control your screen brightness. Persists on reboot.</desc>\n\t\t<image>./icons/Brightness.png</image>\n\t\t<playcount>0</playcount>\n\t\t<lastplayed></lastplayed>\n\t</game>\n\t</gameList>|' /opt/retropie/configs/all/emulationstation/gamelists/retropie/gamelist.xml
fi

if  grep -q "setFreeplayBacklight.sh" /etc/rc.local ; then
	echo "Brightness Control has already been added to rc.local"
else
	sudo sed -i 's|^exit 0|/home/pi/Freeplay/setPCA9633/setFreeplayBacklight.sh > /dev/null\n\nexit 0|' /etc/rc.local
fi

cp $INSTALL_DIR/Brightness.png /home/pi/RetroPie/retropiemenu/icons/Brightness.png

if grep -q "baudrate=400000" /boot/config.txt ; then
	sudo sh -c 'printf "[pi3]\ndtparam=i2c1_baudrate=400000 #makes a big speed difference\ndtoverlay=i2c1-bcm2708,sda1_pin=44,scl1_pin=45,pin_func=6,combine=off\n[all]" >> /boot/config.txt'
else
	echo "Required lines already present in /boot/config.txt"
fi
exit 0
