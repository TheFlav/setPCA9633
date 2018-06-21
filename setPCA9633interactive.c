#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "PCA9633.h"


char getch(){
    /*#include <unistd.h>   //_getch*/
    /*#include <termios.h>  //_getch*/
    char buf=0;
    struct termios old={0};
    fflush(stdout);
    if(tcgetattr(0, &old)<0)
        perror("tcsetattr()");
    old.c_lflag&=~ICANON;
    old.c_lflag&=~ECHO;
    old.c_cc[VMIN]=1;
    old.c_cc[VTIME]=0;
    if(tcsetattr(0, TCSANOW, &old)<0)
        perror("tcsetattr ICANON");
    if(read(0,&buf,1)<0)
        perror("read()");
    old.c_lflag|=ICANON;
    old.c_lflag|=ECHO;
    if(tcsetattr(0, TCSADRAIN, &old)<0)
        perror ("tcsetattr ~ICANON");
    //printf("%c\n",buf);
    return buf;
}

// System dependent key codes
enum
{
    KEY_ESC     = 27,
    KEY_ENTER   = 10,
    ARROW_UP    = 256 + 72,
    ARROW_DOWN  = 256 + 80,
    ARROW_LEFT  = 256 + 75,
    ARROW_RIGHT = 256 + 77
};

static char get_code(void)
{
    int ch = getch();
    
    if(ch == '\033')
    {
        ch = getch();
        if(ch == '[')
            ch = getch();
    }
    
    return ch;
}



int read_dimmer(void)
{
    return PCA9633_get_curr_setting(0 /* LEDn */);
}

void set_dimmer(uint8_t set_val)
{
    struct PCA_regs curr_regs = PCA9633_get_curr_regs();
    
    curr_regs.MODE1 &= 0b11101111;      //WAKE UP!
    PCA9633_write_register(PCA9633_REG_MODE1, curr_regs.MODE1);

    
    if(set_val == 255)
    {
        curr_regs.LEDOUT = PCA9633_generate_new_ledout(curr_regs.LEDOUT, "ON", 0);
        PCA9633_write_register(PCA9633_REG_LEDOUT, curr_regs.LEDOUT);

        curr_regs.MODE1 |= 0b00010000;      //SLEEP
        PCA9633_write_register(PCA9633_REG_MODE1, curr_regs.MODE1);
    }
    else if(set_val == 0)
    {
        curr_regs.LEDOUT = PCA9633_generate_new_ledout(curr_regs.LEDOUT, "OFF", 0);
        PCA9633_write_register(PCA9633_REG_LEDOUT, curr_regs.LEDOUT);
        
        curr_regs.MODE1 |= 0b00010000;      //SLEEP
        PCA9633_write_register(PCA9633_REG_MODE1, curr_regs.MODE1);
    }
    else
    {
        curr_regs.LEDOUT = PCA9633_generate_new_ledout(curr_regs.LEDOUT, "PWM", 0);
        PCA9633_write_register(PCA9633_REG_LEDOUT, curr_regs.LEDOUT);
        
        curr_regs.PWM0 = set_val;
        PCA9633_write_register(PCA9633_REG_PWM0, curr_regs.PWM0);
    }
    
    
    //printf("set_dimmer: set_val=%d\n", set_val);

    return;
}

int init_dimmer()
{
    int rc;
    uint8_t invert_brightness = 0;
    uint8_t save_brightness;
    
    //i2c-1 addr 0x62
    if(PCA9633_init(1, 0x62) != 0)
    {
        exit(EXIT_FAILURE);
    }
    
    if(!PCA9633_is_present())
    {
        exit(EXIT_FAILURE);
    }
    
    
    struct PCA_regs curr_regs = PCA9633_get_curr_regs();

    if(!(curr_regs.MODE2 & 0b00010000))
        invert_brightness = 1;
    
    if(invert_brightness)
        save_brightness = PCA9633_get_curr_setting(0 /* LEDn */);

    curr_regs.MODE2 |= 0b00000100;      //OUTDRV ON
    curr_regs.MODE2 |= 0b00010000;      //INVERT YES
    PCA9633_write_register(PCA9633_REG_MODE2, curr_regs.MODE2);
    
    //if we changed the invert parameter, then
    if(invert_brightness)
        set_dimmer(save_brightness);
    
    return 0;
}

int main (int argc, char **argv)
{
    char downchar, upchar, endchar;
    int stepsize;
    
    if(argc != 5)
    {
        printf("Usage: %s <downchar> <upchar> <endchar> <stepsize>\n", argv[0]);
        return -1;
    }
    
    if(strlen(argv[1]) != 1 || strlen(argv[2]) != 1 || strlen(argv[3]) != 1)
    {
        printf("Invalid argument\n");
        return -1;
    }
    
    downchar = argv[1][0];
    upchar = argv[2][0];
    endchar = argv[3][0];
    stepsize = atoi(argv[4]);
    
    init_dimmer();
    
    double curr_setting = (double)read_dimmer();
    
    
    
    
    int percent = (int)((curr_setting+0.5001)*100.0/255.0);
    //printf("pct=%d, curr_setting=%f\n", percent, curr_setting);
    
#define SINGLE_STEP (256.0 / 100.0)

    if(percent > 100 || percent < 0)
    {
        printf("Bad initial dimmer value %d (%f)\n", percent, curr_setting);
        return -1;
    }
    
    printf("%d\n", percent);
    char key = get_code();
    
    while(key != endchar)
    {
        if(key == upchar)
            percent += stepsize;
        else if(key == downchar)
            percent -= stepsize;
        
        //printf("new %d, %f, l=%f u=%f\n", (int)(curr_setting*100.0/255.0), curr_setting, ((double)stepsize * SINGLE_STEP), (255.0 - ((double)stepsize * SINGLE_STEP)));
        
        if(percent < 0)
            percent = 0;
        
        if(percent > 100)
            percent = 100;

        if(percent == 0)
            curr_setting = 0.0;
        else if(percent == 100)
            curr_setting = 255.0;
        else
            curr_setting = percent * SINGLE_STEP;
        
        set_dimmer((int)(curr_setting+0.5));
        
        //curr_setting = read_dimmer();
        
        //printf("%d, %f %f\n", (int)((curr_setting+0.5001)*100.0/255.0), ((curr_setting+0.5)*100.0/255.0), curr_setting);
        printf("%d\n", percent);
        fflush(stdout);
        
        key = get_code();
    }

    
    return percent;
}

