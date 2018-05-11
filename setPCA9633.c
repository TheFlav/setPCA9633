#include <stdio.h>
#include <fcntl.h>     // open
#include <inttypes.h>  // uint8_t, etc
#include <stdlib.h>
#include <string.h>
#include <argp.h>

#include "PCA9633.h"

/* Program documentation. */
static char doc[] = "Set PCA9553 via i2c, always use -a, only use one of -l, -p, or -s";

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
    {"verbosity",'v', "[0|1|2]",               0,  "0=no output, 1=only output exitcode LEDn value, 2=normal full output" },
    {"exitcode", 'e', "[0|1|2|3]",             0,  "use current LEDn value as exitcode when done" },
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
    
    uint8_t verbosity_level;
    uint8_t exitcode_led;
    
    //    char* setting_str;        //the value to set the configuration to
    //    uint8_t register_already_chosen;
};


/* Parse a single option. */
static error_t parse_opt (int key, char *arg, struct argp_state *state)
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
            
        case 'v' :
            arguments->verbosity_level = strtoul(arg, NULL, 10);
            break;
            
        case 'e' :
            arguments->exitcode_led = strtoul(arg, NULL, 10);
            if(arguments->exitcode_led > 3 || arguments->exitcode_led < 0)
                arguments->exitcode_led = 0;
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



void print_curr_regs(struct PCA_regs curr_regs)
{
    printf("Register Status:\n");
    
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
    arguments.grpfreq_inuse = 0;
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
    
    arguments.verbosity_level = 2;
    arguments.exitcode_led = 0;
    
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
    
    
    PCA9633_init(arguments.i2cbus, arguments.PCA_i2c_address);

    
    struct PCA_regs curr_regs = PCA9633_get_curr_regs();
    
    if(arguments.verbosity_level >= 2)
        print_curr_regs(curr_regs);
    
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
            if(arguments.verbosity_level >= 2)
                printf("Writing MODE1 value of 0x%02X\n", arguments.mode1);
            PCA9633_write_register(PCA9633_REG_MODE1, new_mode1);
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
            if(arguments.verbosity_level >= 2)
                printf("Writing MODE2 value of 0x%02X\n", arguments.mode2);
            PCA9633_write_register(PCA9633_REG_MODE2, new_mode2);
            changes_made++;
        }
    }
    
    
    if(arguments.pwm0_inuse)
    {
        if(arguments.verbosity_level >= 2)
            printf("Writing PWM0 value of 0x%02X\n", arguments.pwm0);
        PCA9633_write_register(PCA9633_REG_PWM0, arguments.pwm0);
        changes_made++;
    }
    
    if(arguments.pwm1_inuse)
    {
        if(arguments.verbosity_level >= 2)
            printf("Writing PWM1 value of 0x%02X\n", arguments.pwm1);
        PCA9633_write_register(PCA9633_REG_PWM1, arguments.pwm1);
        changes_made++;
    }
    
    if(arguments.pwm2_inuse)
    {
        if(arguments.verbosity_level >= 2)
            printf("Writing PWM2 value of 0x%02X\n", arguments.pwm2);
        PCA9633_write_register(PCA9633_REG_PWM2, arguments.pwm2);
        changes_made++;
    }
    
    if(arguments.pwm3_inuse)
    {
        if(arguments.verbosity_level >= 2)
            printf("Writing PWM3 value of 0x%02X\n", arguments.pwm3);
        PCA9633_write_register(PCA9633_REG_PWM3, arguments.pwm3);
        changes_made++;
    }
    
    if(arguments.grppwm_inuse)
    {
        if(arguments.verbosity_level >= 2)
            printf("Writing GRPPWM value of 0x%02X\n", arguments.grppwm);
        PCA9633_write_register(PCA9633_REG_GRPPWM, arguments.grppwm);
        changes_made++;
    }
    
    if(arguments.grpfreq_inuse)
    {
        if(arguments.verbosity_level >= 2)
            printf("Writing GRPFREQ value of 0x%02X\n", arguments.grpfreq);
        
        PCA9633_write_register(PCA9633_REG_GRPFREQ, arguments.grpfreq);
        changes_made++;
    }
    
    uint8_t new_ledout = curr_regs.LEDOUT;
    
    if(arguments.led0_str != NULL)
    {
        new_ledout = PCA9633_generate_new_ledout(new_ledout, arguments.led0_str, 0);
        if(arguments.verbosity_level >= 2)
            printf("LED0: new ledout = 0x%02X\n", new_ledout);
    }
    
    if(arguments.led1_str != NULL)
    {
        new_ledout = PCA9633_generate_new_ledout(new_ledout, arguments.led1_str, 1);
        if(arguments.verbosity_level >= 2)
            printf("LED1: new ledout = 0x%02X\n", new_ledout);
    }
    
    if(arguments.led2_str != NULL)
    {
        new_ledout = PCA9633_generate_new_ledout(new_ledout, arguments.led2_str, 2);
        if(arguments.verbosity_level >= 2)
            printf("LED2: new ledout = 0x%02X\n", new_ledout);
    }
    
    if(arguments.led3_str != NULL)
    {
        new_ledout = PCA9633_generate_new_ledout(new_ledout, arguments.led3_str, 3);
        if(arguments.verbosity_level >= 2)
            printf("LED3: new ledout = 0x%02X\n", new_ledout);
    }
    
    if(new_ledout != curr_regs.LEDOUT)
    {
        PCA9633_write_register(PCA9633_REG_LEDOUT, new_ledout);//LEDOUT
        changes_made++;
    }
    
    if(changes_made)
    {
        curr_regs = PCA9633_get_curr_regs();
        if(arguments.verbosity_level >= 2)
            print_curr_regs(curr_regs);
    }
    else
    {
        if(arguments.verbosity_level >= 2)
            printf("Not updating any registers.\n");
    }
    
    PCA9633_close();
    
    int exitcode = 0;
    int pwm_val = curr_regs.PWM0;
    
    

    exitcode = PCA9633_get_curr_setting(arguments.exitcode_led);
    
    if(arguments.verbosity_level == 1)
        printf("%d\n", PCA9633_get_curr_setting(arguments.exitcode_led));
        
    return exitcode;
    
}
