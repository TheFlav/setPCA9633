#include <stdio.h>
#include <fcntl.h>     // open
#include <inttypes.h>  // uint8_t, etc
#include <linux/i2c-dev.h> // I2C bus definitions
#include <stdlib.h>

#include <argp.h>

int I2CFile;
uint8_t writeBuf[0xFF];      // Buffer to store the 3 bytes that we write to the I2C device
uint8_t readBuf[0xFF];       // Buffer to store the data read from the I2C device

/* Program documentation. */
static char doc[] =
"Set PCA9553 via i2c, always use -a, only use one of -l, -p, or -s";

/* A description of the arguments we accept. */
static char args_doc[] = "VALUE";

/* The options we understand. */
static struct argp_option options[] = {
    {"address",  'a', "0xXX",                  0,  "i2c Address of PCA9553" },
    {"i2cbus",   'y', "[0|1]",                 0,  "use /dev/i2c-0 or /dev/i2c-1 (hint: see i2cdetect command)" },
    {"led0",     'l', "[ON|OFF|PWM|GRP]",      0,  "set led0 to ON, OFF, PWM, or GRP" },
    {"led1",     'm', "[ON|OFF|PWM|GRP]",      0,  "set led1 to ON, OFF, PWM, or GRP" },
    {"led2",     'n', "[ON|OFF|PWM|GRP]",      0,  "set led2 to ON, OFF, PWM, or GRP" },
    {"led3",     'o', "[ON|OFF|PWM|GRP]",      0,  "set led3 to ON, OFF, PWM, or GRP" },
    {"pwm0",     'p', "0xXX",                  0,  "set pwm0 to 0xXX" },
    {"pwm1",     'q', "0xXX",                  0,  "set pwm1 to 0xXX" },
    {"pwm2",     'r', "0xXX",                  0,  "set pwm2 to 0xXX" },
    {"pwm3",     's', "0xXX",                  0,  "set pwm3 to 0xXX" },
    {"grppwm",   'g', "0xXX",                  0,  "set grppwm to 0xXX" },
    {"grpfreq",  'h', "0xXX",                  0,  "set grpfreq to 0xXX" },
    {"mode1",    '1', "0xXX",                  0,  "set mode1 to 0xXX" },
    {"mode2",    '2', "0xXX",                  0,  "set mode1 to 0xXX" },
    {"wake",     'w', "[WAKE|SLEEP]",          0,  "WAKE or SLEEP (no output during SLEEP)" },
    {"invert",   'i', "[NO|YES]",              0,  "Invert PWM? YES/NO" },
    {"outdrv",   'd', "[OFF|ON]",              0,  "OUTDRV on? OFF/ON" },
    {"dmblnk",   'b', "[OFF|ON]",              0,  "DMBLNK on? OFF=dimming/ON=blinking" },
    { 0 }
};

/* Used by main to communicate with parse_opt. */
struct arguments
{
    uint8_t PCA_i2c_address;
    uint8_t i2cbus;
    uint8_t mode1;
    uint8_t mode2;
    
    uint8_t mode1_inuse;
    uint8_t mode2_inuse;
    
    uint8_t pwm0;
    uint8_t pwm1;
    uint8_t pwm2;
    uint8_t pwm3;
    uint8_t grppwm;
    uint8_t grpfreq;
    
    uint8_t pwm0_inuse;
    uint8_t pwm1_inuse;
    uint8_t pwm2_inuse;
    uint8_t pwm3_inuse;
    uint8_t grppwm_inuse;
    uint8_t grpfreq_inuse;
    
    char* led0_str;
    char* led1_str;
    char* led2_str;
    char* led3_str;
    
    char* wake_str;
    char* invert_str;
    char* outdrv_str;
    char* dmblnk_str;
    
    //    char* setting_str;        //the value to set the configuration to
    //    uint8_t register_already_chosen;
};

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
} curr_regs;

/* Parse a single option. */
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
    /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
    struct arguments *arguments = state->input;
    
    /*printf("key=%c\n", key);
     if(arg)
     printf("arg=%s\n", arg);*/
    
    switch (key)
    {
        case 'a':
            arguments->PCA_i2c_address = strtoul(arg, NULL, 16);
            break;
        case 'y':
            arguments->i2cbus = strtoul(arg, NULL, 10);
            if(arguments->i2cbus > 1)
                argp_usage (state);
            break;
        case 'w':
            arguments->wake_str = arg;
            break;
        case 'i':
            arguments->invert_str = arg;
            break;
        case 'd':
            arguments->outdrv_str = arg;
            break;
        case 'b':
            arguments->dmblnk_str = arg;
            break;
        case 'l':
            arguments->led0_str = arg;
            break;
        case 'm':
            arguments->led1_str = arg;
            break;
        case 'n':
            arguments->led2_str = arg;
            break;
        case 'o':
            arguments->led3_str = arg;
            break;
        case '1':
            arguments->mode1 = strtoul(arg, NULL, 16);
            arguments->mode1_inuse = 1;
            break;
        case '2':
            arguments->mode2 = strtoul(arg, NULL, 16);
            arguments->mode2_inuse = 1;
            break;
            
        case 'p':
            arguments->pwm0 = strtoul(arg, NULL, 16);
            arguments->pwm0_inuse = 1;
            //printf("PWM0: 0x%02X\n", arguments->pwm0);
            break;
        case 'q':
            arguments->pwm1 = strtoul(arg, NULL, 16);
            arguments->pwm1_inuse = 1;
            //printf("PWM1: 0x%02X\n", arguments->pwm1);
            break;
        case 'r':
            arguments->pwm2 = strtoul(arg, NULL, 16);
            arguments->pwm2_inuse = 1;
            //printf("PWM2: 0x%02X\n", arguments->pwm2);
            break;
        case 's':
            arguments->pwm3 = strtoul(arg, NULL, 16);
            arguments->pwm3_inuse = 1;
            //printf("PWM3: 0x%02X\n", arguments->pwm3);
            break;
        case 'g':
            arguments->grppwm = strtoul(arg, NULL, 16);
            arguments->grppwm_inuse = 1;
            break;
        case 'h':
            arguments->grpfreq = strtoul(arg, NULL, 16);
            arguments->grpfreq_inuse = 1;
            break;
            
            
        case ARGP_KEY_ARG:
            if (state->arg_num >= 0)
            /* Too many arguments. */
                argp_usage (state);
            
            //arguments->setting_str = arg;
            
            break;
            
        case ARGP_KEY_END:
            if (state->arg_num < 0)
            /* Not enough arguments. */
                argp_usage (state);
            break;
            
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };


uint8_t generate_new_ledout(uint8_t old_ledout, char *led_str, uint8_t led_num)
{
    
    if(led_num > 3)
        return old_ledout;
    
    /*
     LDRx = 00 — LED driver x is off (default power-up state).
     LDRx = 01 — LED driver x is fully on (individual brightness and group dimming/blinking not controlled).
     LDRx = 10 — LED driver x individual brightness can be controlled through its PWMx register.
     LDRx = 11 — LED driver x individual brightness and group dimming/blinking can be controlled through its PWMx register and the GRPPWM registers.
     */
    
    uint8_t ls_val=0xFF;
    if(strcmp(led_str, "PWM") == 0)
        ls_val = 0b10;
    else if(strcmp(led_str, "GRP") == 0)
        ls_val = 0b11;
    else if(strcmp(led_str, "OFF") == 0)
        ls_val = 0b00;
    else if(strcmp(led_str, "ON") == 0)
        ls_val = 0b01;
    else
    {
        printf("Invalid led string for LED%d\n", led_num);
        exit(1);
    }
    
    printf("LED%d being set to %d\n", led_num, ls_val);
    
    return (old_ledout & (~(0x03 << (led_num*2)))) | (ls_val << (led_num*2));
}

void get_curr_regs(void)
{
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
}

void print_curr_regs(void)
{
    printf("  MODE1   0x%02X (use -1 to set)\n", curr_regs.MODE1);
    printf("  MODE2   0x%02X (use -2 to set)\n", curr_regs.MODE2);
    
    printf("  PWM0    0x%02X (use -p to set)\n", curr_regs.PWM0);
    printf("  PWM1    0x%02X (use -q to set)\n", curr_regs.PWM1);
    printf("  PWM2    0x%02X (use -r to set)\n", curr_regs.PWM2);
    printf("  PWM3    0x%02X (use -s to set)\n", curr_regs.PWM3);
    
    printf("  GRPPWM  0x%02X (use -g to set)\n", curr_regs.GRPPWM);
    printf("  GRPFREQ 0x%02X (use -h to set)\n", curr_regs.GRPFREQ);
    
    printf("  LEDOUT  0x%02X (use -l,-m,-n,-o to set)\n", curr_regs.LEDOUT);
    
    printf("    LED0 %4s (use -l to set)\n", curr_regs.LEDOUT & 0b10    ? (curr_regs.LEDOUT    & 0b01 ? "GRP" : "PWM") : (curr_regs.LEDOUT & 0b01 ? "ON" : "OFF"));
    printf("    LED1 %4s (use -m to set)\n", curr_regs.LEDOUT>>2 & 0b10 ? (curr_regs.LEDOUT>>2 & 0b01 ? "GRP" : "PWM") : (curr_regs.LEDOUT>>2 & 0b01 ? "ON" : "OFF"));
    printf("    LED2 %4s (use -n to set)\n", curr_regs.LEDOUT>>4 & 0b10 ? (curr_regs.LEDOUT>>4 & 0b01 ? "GRP" : "PWM") : (curr_regs.LEDOUT>>4 & 0b01 ? "ON" : "OFF"));
    printf("    LED3 %4s (use -o to set)\n", curr_regs.LEDOUT>>6 & 0b10 ? (curr_regs.LEDOUT>>6 & 0b01 ? "GRP" : "PWM") : (curr_regs.LEDOUT>>6 & 0b01 ? "ON" : "OFF"));
}

int main (int argc, char **argv)
{
    struct arguments arguments;
    
    /* Default values. */
    arguments.PCA_i2c_address = 0x62;   // Address of our device on the I2C bus
    arguments.pwm0_inuse = 0;
    arguments.pwm1_inuse = 0;
    arguments.pwm2_inuse = 0;
    arguments.pwm3_inuse = 0;
    arguments.grppwm_inuse = 0;
    arguments.mode1_inuse = 0;
    arguments.mode2_inuse = 0;
    arguments.i2cbus = 1;
    //arguments.setting_str = NULL;
    arguments.invert_str = NULL;
    arguments.outdrv_str = NULL;
    arguments.dmblnk_str = NULL;
    arguments.wake_str = NULL;
    arguments.led0_str = NULL;
    arguments.led1_str = NULL;
    arguments.led2_str = NULL;
    arguments.led3_str = NULL;
    
    /* Parse our arguments; every option seen by parse_opt will
     be reflected in arguments. */
    argp_parse (&argp, argc, argv, 0, 0, &arguments);
    
    /*printf ("SETTING = %s\nADDR = 0x%02X\n"
     "PWM = %d\nPSC = %d\nLED = %d\n",
     arguments.setting_str ? arguments.setting_str : "",
     arguments.PCA_i2c_address,
     arguments.pwm,
     arguments.psc,
     arguments.led);*/
    
    
    if(arguments.i2cbus == 1)
        I2CFile = open("/dev/i2c-1", O_RDWR);     // Open the I2C device
    else
        I2CFile = open("/dev/i2c-0", O_RDWR);     // Open the I2C device
    
    ioctl(I2CFile, I2C_SLAVE, arguments.PCA_i2c_address);   // Specify the address of the I2C Slave to communicate with
    
    printf("Current Register Status:\n");
    get_curr_regs();
    print_curr_regs();
    
    //    printf("  LEDOUT   0x%02X (use -l,-m,-n,-o to set)\n", curr_regs.LEDOUT);
    
    uint8_t changes_made = 0;
    
    if(arguments.mode1_inuse || arguments.wake_str != NULL)
    {
        uint8_t new_mode1 = 0;
        
        if(arguments.mode1_inuse)
            new_mode1 = arguments.mode1;
        else
            new_mode1 = curr_regs.MODE1;
        
        if(arguments.wake_str)
        {
            if(strcmp(arguments.wake_str, "WAKE") == 0)
                new_mode1 &= 0b11101111;
            else if(strcmp(arguments.wake_str, "SLEEP") == 0)
                new_mode1 |= 0b00010000;
            
        }
        
        if(new_mode1 != curr_regs.MODE1)
        {
            printf("Writing MODE1 value of 0x%02X\n", arguments.mode1);
            writeBuf[0] = 0b00000000;
            writeBuf[1] = new_mode1;
            writeBuf[2] = 0;
            write(I2CFile, writeBuf, 2);
            changes_made++;
        }
    }
    
    if(arguments.mode2_inuse || arguments.invert_str != NULL || arguments.outdrv_str != NULL || arguments.dmblnk_str != NULL)
    {
        uint8_t new_mode2 = 0;
        
        if(arguments.mode2_inuse)
            new_mode2 = arguments.mode2;
        else
            new_mode2 = curr_regs.MODE2;
        
        if(arguments.invert_str)
        {
            if(strcmp(arguments.invert_str, "NO") == 0)
                new_mode2 &= 0b11101111;
            else if(strcmp(arguments.invert_str, "YES") == 0)
                new_mode2 |= 0b00010000;
            
        }
        
        if(arguments.outdrv_str)
        {
            if(strcmp(arguments.outdrv_str, "OFF") == 0)
                new_mode2 &= 0b11111011;
            else if(strcmp(arguments.outdrv_str, "ON") == 0)
                new_mode2 |= 0b00000100;
        }
        
        if(arguments.dmblnk_str)
        {
            if(strcmp(arguments.dmblnk_str, "OFF") == 0)
                new_mode2 &= 0b11011111;
            else if(strcmp(arguments.dmblnk_str, "ON") == 0)
                new_mode2 |= 0b00100000;
        }
        
        if(new_mode2 != curr_regs.MODE2)
        {
            printf("Writing MODE2 value of 0x%02X\n", arguments.mode2);
            writeBuf[0] = 0b00000001;
            writeBuf[1] = new_mode2;
            writeBuf[2] = 0;
            write(I2CFile, writeBuf, 2);
            changes_made++;
        }
    }
    
    
    if(arguments.pwm0_inuse)
    {
        printf("Writing PWM0 value of 0x%02X\n", arguments.pwm0);
        writeBuf[0] = 0b00000010;
        writeBuf[1] = arguments.pwm0;
        writeBuf[2] = 0;
        write(I2CFile, writeBuf, 2);
        changes_made++;
    }
    
    if(arguments.pwm1_inuse)
    {
        printf("Writing PWM1 value of 0x%02X\n", arguments.pwm1);
        writeBuf[0] = 0b00000011;
        writeBuf[1] = arguments.pwm1;
        writeBuf[2] = 0;
        write(I2CFile, writeBuf, 2);
        changes_made++;
    }
    
    if(arguments.pwm2_inuse)
    {
        printf("Writing PWM2 value of 0x%02X\n", arguments.pwm2);
        writeBuf[0] = 0b00000100;
        writeBuf[1] = arguments.pwm2;
        writeBuf[2] = 0;
        write(I2CFile, writeBuf, 2);
        changes_made++;
    }
    
    if(arguments.pwm3_inuse)
    {
        printf("Writing PWM3 value of 0x%02X\n", arguments.pwm3);
        writeBuf[0] = 0b00000101;
        writeBuf[1] = arguments.pwm3;
        writeBuf[2] = 0;
        write(I2CFile, writeBuf, 2);
        changes_made++;
    }
    
    if(arguments.grppwm_inuse)
    {
        printf("Writing GRPPWM value of 0x%02X\n", arguments.grppwm);
        writeBuf[0] = 0b00000110;
        writeBuf[1] = arguments.grppwm;
        writeBuf[2] = 0;
        write(I2CFile, writeBuf, 2);
        changes_made++;
    }
    
    if(arguments.grpfreq_inuse)
    {
        printf("Writing GRPFREQ value of 0x%02X\n", arguments.grpfreq);
        writeBuf[0] = 0b00000111;
        writeBuf[1] = arguments.grpfreq;
        writeBuf[2] = 0;
        write(I2CFile, writeBuf, 2);
        changes_made++;
    }
    
    uint8_t new_ledout = curr_regs.LEDOUT;
    
    if(arguments.led0_str != NULL)
    {
        new_ledout = generate_new_ledout(new_ledout, arguments.led0_str, 0);
        printf("LED0: new ledout = 0x%02X\n", new_ledout);
    }
    
    if(arguments.led1_str != NULL)
    {
        new_ledout = generate_new_ledout(new_ledout, arguments.led1_str, 1);
        //printf("LED1: new ledout = 0x%02X\n", new_ledout);
    }
    
    if(arguments.led2_str != NULL)
    {
        new_ledout = generate_new_ledout(new_ledout, arguments.led2_str, 2);
        //printf("LED2: new ledout = 0x%02X\n", new_ledout);
    }
    
    if(arguments.led3_str != NULL)
    {
        new_ledout = generate_new_ledout(new_ledout, arguments.led3_str, 3);
        //printf("LED3: new ledout = 0x%02X\n", new_ledout);
    }
    
    if(new_ledout != curr_regs.LEDOUT)
    {
        writeBuf[0] = 0b00001000;       //LEDOUT
        writeBuf[1] = new_ledout;
        writeBuf[2] = 0;
        write(I2CFile, writeBuf, 2);
        changes_made++;
    }
    
    if(changes_made)
    {
        printf("New Register Status:\n");
        get_curr_regs();
        print_curr_regs();
    }
    else
        printf("Not updating any registers.\n");
    
    close(I2CFile);
    
    return 0;
    
}
