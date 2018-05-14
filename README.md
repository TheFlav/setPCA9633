# setPCA9633
Set PCA9633 via i2c on Raspberry Pi
Raspberry Pi executable to set the NXP PCA9633

make
sudo raspi-config   (then under interface options, enable I2C and reboot)





# For Freeplay Zero/CM3
cp "Freeplay LCD Brightness.sh" /home/pi/RetroPie/retropiemenu/
echo "/home/pi/Freeplay/setPCA9633/setFreeplayBacklight.sh > /dev/null" > /boot/freeplaycfg.txt
