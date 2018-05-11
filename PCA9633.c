#include <stdio.h>
#include <fcntl.h>     // open
#include <linux/i2c-dev.h> // I2C bus definitions
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "PCA9633.h"


int I2CFile = 0;
uint8_t writeBuf[0xFF];      // Buffer to store the 3 bytes that we write to the I2C device
uint8_t readBuf[0xFF];       // Buffer to store the data read from the I2C device




int PCA9633_init(uint8_t i2cbusnum, uint8_t i2caddress)
{
    
    if(i2cbusnum == 1)
        I2CFile = open("/dev/i2c-1", O_RDWR);     // Open the I2C device
    else
        I2CFile = open("/dev/i2c-0", O_RDWR);     // Open the I2C device
    
    ioctl(I2CFile, I2C_SLAVE, i2caddress);   // Specify the address of the I2C Slave to communicate with
    
    return 0;
}

uint8_t PCA9633_generate_new_ledout(uint8_t old_ledout, char *led_str, uint8_t led_num)
{
    
    if(led_num > 3)
        return old_ledout;
    
    /*
     LDRx = 00 — LED driver x is off (default power-up state).
     LDRx = 01 — LED driver x is fully on (individual brightness and group dimming/blinking not controlled).
     LDRx = 10 — LED driver x individual brightness can be controlled through its PWMx register.
     LDRx = 11 — LED driver x individual brightness and group dimming/blinking can be controlled through its PWMx register and the GRPPWM registers.
     */
    
    //printf("SETTING: %s\n", led_str);
    
    uint8_t ls_val=0xFF;
    if(strcmp(led_str, "PWM") == 0)
    {
        //printf("setting to PWM\n");
        ls_val = 0b10;
    }
    else if(strcmp(led_str, "GRP") == 0)
    {
        //printf("setting to GRP\n");
        ls_val = 0b11;
    }
    else if(strcmp(led_str, "OFF") == 0)
    {
        //printf("setting to OFF\n");
        ls_val = 0b00;
    }
    else if(strcmp(led_str, "ON") == 0)
    {
        //printf("setting to ON\n");
        ls_val = 0b01;
    }
    else
    {
        printf("Invalid led string for LED%d\n", led_num);
        exit(1);
    }
    
    uint8_t led_mask = 0x03 << (led_num*2);
    uint8_t masked_ledout = old_ledout & ~led_mask;
    uint8_t led_bits = ls_val << (led_num*2);
    uint8_t new_ledout = masked_ledout | led_bits;
    //printf("old_ledout=0x%02X masked_ledout=0x%02X led_bits=0x%02X new_ledout=0x%02X\n", old_ledout, masked_ledout, led_bits, new_ledout);
                        
    
    return new_ledout;
}

struct PCA_regs PCA9633_get_curr_regs(void)
{
    struct PCA_regs curr_regs;

    writeBuf[0] = 0x80;   //set auto-increment for reading all registers
    write(I2CFile, writeBuf, 1);
    
    read(I2CFile, readBuf, 0x0D);
    
    curr_regs.MODE1  = readBuf[0];
    curr_regs.MODE2  = readBuf[1];
    curr_regs.PWM0   = readBuf[2];
    curr_regs.PWM1   = readBuf[3];
    curr_regs.PWM2   = readBuf[4];
    curr_regs.PWM3   = readBuf[5];
    curr_regs.GRPPWM    = readBuf[6];
    curr_regs.GRPFREQ   = readBuf[7];
    curr_regs.LEDOUT    = readBuf[8];
    curr_regs.SUBADR1   = readBuf[9];
    curr_regs.SUBADR2   = readBuf[0x0A];
    curr_regs.SUBADR3   = readBuf[0x0B];
    curr_regs.ALLCALLADR= readBuf[0x0C];
    
    writeBuf[0] = 0x00;   //don't set auto-increment for reading input port
    write(I2CFile, writeBuf, 1);
    
    read(I2CFile, readBuf, 1);
    curr_regs.MODE1  = readBuf[0];
    
    return curr_regs;
}

uint8_t PCA9633_get_curr_setting(uint8_t led_num)
{
    struct PCA_regs curr_regs = PCA9633_get_curr_regs();
    uint8_t led_val = 0;
    
    uint8_t led_bits = (curr_regs.LEDOUT >> (led_num * 2)) & 0b11;
    
    
    /*
     LDRx = 00 — LED driver x is off (default power-up state).
     LDRx = 01 — LED driver x is fully on (individual brightness and group dimming/blinking not controlled).
     LDRx = 10 — LED driver x individual brightness can be controlled through its PWMx register.
     LDRx = 11 — LED driver x individual brightness and group dimming/blinking can be controlled through its PWMx register and the GRPPWM registers.
     */
    
    //printf("led_bits=%d, curr_regs.PWM0=%d\n", led_bits, curr_regs.PWM0);
    
    switch(led_bits)
    {
        
        case 0b00:
            led_val = 0;
            break;
        case 0b01:
        default:
            led_val = 255;
            break;
        case 0b10:
            switch(led_num)
            {
                default:
                case 0:
                    led_val = curr_regs.PWM0;
                    break;
                case 1:
                    led_val = curr_regs.PWM1;
                    break;
                case 2:
                    led_val = curr_regs.PWM2;
                    break;
                case 3:
                    led_val = curr_regs.PWM3;
                    break;
            }
            break;
        case 0b11:
            led_val = curr_regs.GRPPWM;
            break;
    }
    
    if(!(curr_regs.MODE2 & 0b00010000))
    {
        /* MODE2 bit 4
         INVRT[1] R/W 0* Output logic state not inverted. Value to use when no external driver used.
         Applicable when OE = 0 for PCA9633 16-pin version.
         1 Output logic state inverted. Value to use when external driver used.
         Applicable when OE = 0 for PCA9633 16-pin version.
         */
        led_val = 255 - led_val;
    }
    
    return led_val;
}


void PCA9633_write_register(uint8_t reg_num, uint8_t reg_val)
{
    writeBuf[0] = reg_num;
    writeBuf[1] = reg_val;
    writeBuf[2] = 0;
    write(I2CFile, writeBuf, 2);
}

void PCA9633_close(void)
{
    close(I2CFile);
}
