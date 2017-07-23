#include <p24Fxxxx.h>
#include <xc.h>
#include <libpic30.h>
#include <stdio.h>
#include "../Project.X/Header_file.h"
#include <string.h>
#include <assert.h>
#include <math.h>


volatile unsigned int cbuffer[1024];
volatile unsigned int avbuffer[1024];
volatile unsigned int last32[32];

volatile unsigned int front = 0;
volatile unsigned int avfront = 0;
volatile unsigned int last32front = 0;

void put_soil(unsigned int x){   
    cbuffer[front++] = x;
    front &= 1023;
    last32[last32front++] = x;
    last32front &= 31;
}
void put_av_soil(unsigned int x){   
    avbuffer[avfront++] = x;
    avfront &= 1023;
}
unsigned int get_soil(void){
    unsigned int x;
    x = cbuffer[front - 1];
    front--;
    front &= 1023;
    return x;
}
unsigned int get_av_soil(void){
    unsigned int avg;
    avg = avbuffer[avfront - 1];
    avfront--;
    avfront &= 1023;
    return avg;
}

double get_moisture_soil(void){
    unsigned int avg;
    double moisture;
    avg = avbuffer[avfront - 1];
    moisture = 100.0*avg/800; //past 2.58V is pure water 2.58/3.3*1023 = 800
    if (moisture > 100) {moisture = 100;}
    return moisture;
}

void __attribute__((__interrupt__,__auto_psv__)) _T1Interrupt(void){
    _T1IF = 0;
    unsigned int sum = 0 , i, mean = 0; // standard = 0;
    for (i=0 ; i<32 ; i++) { // wIdx might move as we do comp
        sum += last32[i];
    }
    mean = sum/32;
    put_av_soil(mean);
}

void __attribute__((__interrupt__)) _ADC1Interrupt(void) {
   _AD1IF = 0;
    put_soil(ADC1BUF0);
}
void soilSetup(void) {
    
    AD1CON1 = 0;
    AD1CON2 = 0;
    AD1CON3 = 0;
    AD1CHS = 0;
    
    AD1CON1bits.ASAM = 1;       //Auto sampling
    AD1CON1bits.SSRC = 0b010;   //Use timer 3 for auto sample
    AD1CON3bits.ADCS = 1;
    
    T3CON = 0;
    TMR3 = 0;
    T3CONbits.TCKPS1 = 1;       //set to 64
    PR3 = 15625;
    
    _AD1IF = 0;
    _AD1IE = 1;
    
    //For standard div
    T1CON = 0;
    T1CONbits.TCKPS1 = 1;
    PR1 = 15625*2;
    _T1IF = 0;
    _T1IP = 2;
    _T1IE = 1;
    T1CONbits.TON = 1;
    
    AD1CON1bits.ADON = 1;
    T3CONbits.TON = 1;
}