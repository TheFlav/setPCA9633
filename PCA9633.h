#ifndef PCA9633_H
#define PCA9633_H

#include <inttypes.h>  // uint8_t, etc


uint8_t PCA9633_generate_new_ledout(uint8_t old_ledout, char *led_str, uint8_t led_num);
struct PCA_regs PCA9633_get_curr_regs(void);
void PCA9633_write_register(uint8_t reg_num, uint8_t reg_val);
int PCA9633_init(uint8_t i2cbusnum, uint8_t i2caddress);
void PCA9633_close(void);

struct PCA_regs
{
    uint8_t MODE1;
    uint8_t MODE2;
    uint8_t PWM0;
    uint8_t PWM1;
    uint8_t PWM2;
    uint8_t PWM3;
    uint8_t GRPPWM;
    uint8_t GRPFREQ;
    uint8_t LEDOUT;
    uint8_t SUBADR1;
    uint8_t SUBADR2;
    uint8_t SUBADR3;
    uint8_t ALLCALLADR;
};

#define PCA9633_REG_MODE1   0b00000000
#define PCA9633_REG_MODE2   0b00000001
#define PCA9633_REG_PWM0    0b00000010
#define PCA9633_REG_PWM1    0b00000011
#define PCA9633_REG_PWM2    0b00000100
#define PCA9633_REG_PWM3    0b00000101
#define PCA9633_REG_GRPPWM  0b00000110
#define PCA9633_REG_GRPFREQ 0b00000111
#define PCA9633_REG_LEDOUT  0b00001000

#endif
