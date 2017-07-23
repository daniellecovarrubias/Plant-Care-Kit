
#include <p24Fxxxx.h>
#include <xc.h>
#include <libpic30.h>
#include <stdio.h>
#include "../Project.X/Header_file.h"
#include <string.h>
#include <assert.h>
#include <math.h>

//Some of this code was translated by the arduino library that is provided with the device
//The code is found at https://github.com/adafruit/Adafruit_SI1145_Library/blob/master/Adafruit_SI1145.cpp#L54

char in_light_bool = 0; 
int minute = 0;
int hour = 0;
int total_minute_light = 0;
int total_minute_nolight = 0;

void __attribute__((__interrupt__,__auto_psv__)) _T2Interrupt(void){
    _T2IF = 0;
    //We are assuming that if the UV index is > 0, then there is sunlight present
    static int count = 0;
    count++;
    static int sample  = 0;
    sample+=get_uv_index();
    if(count >= 60){
        sample = sample/60;//get average uv for a one second range
        if(sample > 0){
        //It is now in light
            if(in_light_bool){
                //already in light so inc
                minute++;
                total_minute_light++;
            }else{          
                //Change it in light
                in_light_bool = 1;
                minute = 0;
                hour = 0;
            }
        }else{
            if(in_light_bool){
                //was in light. now not.
                in_light_bool = 0;
                minute = 0;
                hour = 0;
            }else{
                //was not in light.  still not.
                minute++;
                total_minute_nolight++;
            }
        }        
        if (minute>=60){
            hour++;
            minute = 0;
        }
        count = 0;
        sample = 0;
    }
}
void uv_setup(void) {
     
    TMR2 = 0;
    T2CONbits.TCKPS0 = 1;
    T2CONbits.TCKPS1 = 1;       //256 mult
    PR2 = 62500;                //1 second
    _T2IF = 0;
    _T2IE = 1;
    T2CONbits.TON = 1;
    
    I2C1CONbits.I2CEN = 0;
    I2C1BRG = 157;
    _MI2C1IF = 0;
    I2C1CONbits.A10M = 0;   //Set to 7 bits
    I2C1CONbits.ACKDT = 1;  //Set host to send NACK
    I2C1CONbits.I2CEN = 1;
    
    uv_init();
}
void uv_cmd_write(char reg,char data){
    I2C1CONbits.SEN = 1;
    while(I2C1CONbits.SEN); //S
    
    _MI2C1IF = 0;
    I2C1TRN = 0xC0;         //Client Id + 0
                            //Write = 0xC0, read = 0xC1
    while(!_MI2C1IF);       //acknowledge
    
    _MI2C1IF = 0;   
    I2C1TRN = reg;          //register address
    while(!_MI2C1IF);       //Acknowledge
    
    _MI2C1IF = 0;
    I2C1TRN = data;         //Data
    while(!_MI2C1IF);       //Acknowledge
    
    _MI2C1IF = 0;
    I2C1CONbits.PEN = 1;    //Stop condition
    while(I2C1CONbits.PEN);

}
int uv_cmd_read(char reg){
    I2C1CONbits.SEN = 1;
    while(I2C1CONbits.SEN); //S
    
    _MI2C1IF = 0;
    I2C1TRN = 0xC0;         //Client Id + 0
                            //Write = 0xC0, read = 0xC1
    while(!_MI2C1IF);       //acknowledge
    
    _MI2C1IF = 0;   
    I2C1TRN = reg;          //register address
    while(!_MI2C1IF);       //Acknowledge
    
    I2C1CONbits.RSEN = 1;
    while(I2C1CONbits.RSEN); //S
    
    _MI2C1IF = 0;
    I2C1TRN = 0xC1;         //read
    while(!_MI2C1IF);       //acknowledge
    
    I2C1CONbits.RCEN = 1;   //Enable receive.  Goes to IC1RCV
    while(_RCEN);           //While still receiving

    I2C1CONbits.ACKEN = 1;  //Send NACK
    while(_ACKEN);
    
    I2C1CONbits.PEN = 1;    //Send stop
    while(I2C1CONbits.PEN);
    
    return I2C1RCV;
}
void uv_init(void){
    delay(25);                                  //Start up time
    uv_cmd_write(SI1145_REG_HWKEY, 0x17);        //Must do this for proper start up
    uv_cmd_write(SI1145_REG_UCOEFF0, 0x29);     //Default UV setting values
    uv_cmd_write(SI1145_REG_UCOEFF1, 0x89);
    uv_cmd_write(SI1145_REG_UCOEFF2, 0x02);
    uv_cmd_write(SI1145_REG_UCOEFF3, 0x00);
 
    //enable UV, ALS IR, and ALS Visible sensor
    uv_cmd_write_parameter(SI1145_PARAM_CHLIST, SI1145_PARAM_CHLIST_ENUV | SI1145_PARAM_CHLIST_ENALSIR | SI1145_PARAM_CHLIST_ENALSVIS);
    //Set automeasure to longest amount of time
    uv_cmd_write(SI1145_REG_MEASRATE0, 0xFF);// this will sample every 31.5 us * FF = roughly 8ms
    //starts the auto sampling
    uv_cmd_write(SI1145_REG_COMMAND, SI1145_PSALS_AUTO);
}
int uv_cmd_write_parameter(char a, char b){
    uv_cmd_write(SI1145_REG_PARAMWR, b);
    uv_cmd_write(SI1145_REG_COMMAND, a | SI1145_PARAM_SET);
    return uv_cmd_read(SI1145_REG_PARAMRD);
}
int uv_cmd_read16(char reg){
    int x;
    I2C1CONbits.SEN = 1;
    while(I2C1CONbits.SEN); //S
    
    _MI2C1IF = 0;
    I2C1TRN = 0xC0;         //Client Id + 0
                            //Write = 0xC0, read = 0xC1
    while(!_MI2C1IF);       //acknowledge
    
    _MI2C1IF = 0;   
    I2C1TRN = reg;          //register address
    while(!_MI2C1IF);       //Acknowledge
    
    I2C1CONbits.RSEN = 1;
    while(I2C1CONbits.RSEN); //S
     
    _MI2C1IF = 0;
    I2C1TRN = 0xC1;         //read
    while(!_MI2C1IF);       //acknowledge
    
    I2C1CONbits.RCEN = 1;   //Enable receive.  Goes to IC1RCV
    while(_RCEN);           //While still receiving
    
    x = I2C1RCV;
    I2C1CONbits.ACKDT = 0;  //Set host to send ACK
   
    I2C1CONbits.ACKEN = 1;  //Send ACK
    while(_ACKEN);
    I2C1CONbits.ACKDT = 1; //Set back to NACK
    
    I2C1CONbits.RCEN = 1;   //Enable receive.  Goes to IC1RCV
    while(_RCEN);           //While still receiving
    x |= I2C1RCV << 8;
    
    I2C1CONbits.ACKEN = 1;  //Send NACK
    while(_ACKEN);
    
    I2C1CONbits.PEN = 1;    //Send stop
    while(I2C1CONbits.PEN);
    
    return x;

}
int get_uv_index(void){
    //uv_cmd_write(SI1145_REG_COMMAND, SI1145_ALS_FORCE); //Force a sampling
    return uv_cmd_read16(0x2C)/100;
}
int get_visible_light(void){
    //uv_cmd_write(SI1145_REG_COMMAND, SI1145_ALS_FORCE); //Force a sampling
    return uv_cmd_read16(0x22);
}
int get_IR(void){
    return uv_cmd_read16(0x24);
}
char in_light(){
    return in_light_bool;
}
int get_hour(){
    return hour;
}
int get_minute(){
    return minute;
}
int total_percent_inlight(){
    if(total_minute_light>0){
        return (100*total_minute_light)/(total_minute_light + total_minute_nolight);
    }else{
        return 0;
    }
}