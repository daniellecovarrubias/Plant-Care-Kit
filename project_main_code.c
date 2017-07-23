#include <p24Fxxxx.h>
#include <xc.h>
#include "stdint.h"
#include "../Project.X/Header_file.h"
#include <stdio.h>
#include <string.h>

#pragma config POSCMOD = NONE           // Primary Oscillator Select (Primary oscillator disabled)
#pragma config I2C1SEL = PRI            // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF            // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = ON            // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as port I/O (RC15))
#pragma config FCKSM = CSECME           // Clock Switching and Monitor (Clock switching is enabled, Fail-Safe Clock Monitor is enabled)
#pragma config FNOSC = FRCPLL           // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))
#pragma config SOSCSEL = SOSC           // Sec Oscillator Select (Default Secondary Oscillator (SOSC))
#pragma config WUTSEL = LEG             // Wake-up timer Select (Legacy Wake-up Timer)
#pragma config IESO = ON                // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) enabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = ON              // Watchdog Timer Window (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config ICS = PGx1               // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)

//function definitions
volatile int mode=0; //LCD mode

void __attribute__((__interrupt__,__auto_psv__)) _INT0Interrupt(void)
{
    //INT0 interrupt that cycles through the modes when the pin receives a pulse
    _INT0IF = 0;
   
    if (mode>=4) //cycles back to the first mode
        mode=0;     
    else
        mode++;
    
}

void main_setup(){
    CLKDIVbits.RCDIV = 0;
    AD1PCFG = 0xfffe; // AN0 is the only analog pin we are using.
    
    // setup INT0 (RB7) pin
    TRISB=0; //initialize pins as outputs
    TRISBbits.TRISB7 = 1; // make RB7 an input pin. RB7 and INT0 pin are multiplexed.
    LATBbits.LATB7=0; //set INT0 pin low
    _INT0EP = 0;  // edge polarity set to detection of positive edge.
    _INT0IF = 0;  // reset so that you don't respond to a previously generated event.
    _INT0IE = 1;  // enable interrupt for INT0 events.
    
}

void lcd_mode(){
    char str[8]; //String that is outputted on the LCD screen
    char hour[8]; //stores the total hours in the sun as a string
    char min[4]; //stores the total minutes in the sun as a string
    if (mode==0){ //sleep mode
        lcd_clear();
    }
    else if (mode==1){ //UV index mode
        lcd_clear();
        lcd_printStr(0,0,"UV Index");
        sprintf(str,"%8i", get_uv_index());
        lcd_printStr(1,0,str);    
    }
    else if (mode==2){ //time in sun mode
        lcd_clear();
        if(in_light()){
            lcd_printStr(0,0,"In Light");
        }
        else{
            lcd_printStr(0,0,"No Light");
        }
        sprintf(hour,"%2i h", get_hour());
        sprintf(min,"%2i m", get_minute());
        strcpy(str,hour);
        strcat(str, min);
        lcd_printStr(1,0,str);
        
    }
    else if (mode==3){ //percent in sun mode
        lcd_clear();
        
        lcd_printStr(0,0,"In Light");
        sprintf(str,"%7i%%", total_percent_inlight());
        lcd_printStr(1,0,str);   
       
    }
    else{//soil percent mode
        lcd_clear();
        lcd_printStr(0,0,"Moisture");
        sprintf(str,"%6.2f%%", get_moisture_soil());
        lcd_printStr(1,0,str);          
    }
        
}

int main(void) {
    main_setup(); //calls setup above
    lcd_setup(); //calls from LCD library
    iLEDsetup(); //calls from the iLED library
    uv_setup(); //calls form the UV library
    soilSetup(); //calls from the soil library
    
    
    while(1){
        delay(1000); //slows down the amount of times the LCD and iLED is refreshed
        lcd_mode(); //displays the mode of the LCD
        iLEDsun(get_uv_index()); //sends a color to the sun iLED
        double double_moisture = get_moisture_soil();
        int moisture = (int) double_moisture;
        iLEDsoil(moisture); //sends a color to the soil iLED
    } 
    return 0;
}
