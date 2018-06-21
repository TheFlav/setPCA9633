# setPCA9633
<<<<<<< HEAD
Set PCA9633 via i2c on Raspberry Pi
Raspberry Pi executable to set the NXP PCA9633

make

sudo raspi-config   (then under interface options, enable I2C and reboot)





# For Freeplay Zero/CM3
cp "Freeplay LCD Brightness.sh" /home/pi/RetroPie/retropiemenu/
echo "/home/pi/Freeplay/setPCA9633/setFreeplayBacklight.sh > /dev/null" > /boot/freeplaycfg.txt
=======
Set PCA9633 via i2c on Raspberry Pi<br />
Raspberry Pi executable to set the NXP PCA9633<br />
<br />
make<br />
sudo raspi-config   (then under interface options, enable I2C and reboot)<br />
<br />
<br />
<br />
<br />
<br />
# For Freeplay Zero/CM3<br />
cp "Freeplay LCD Brightness.sh" /home/pi/RetroPie/retropiemenu/<br />
echo "/home/pi/Freeplay/setPCA9633/setFreeplayBacklight.sh > /dev/null" > /boot/freeplaycfg.txt<br />
>>>>>>> master
