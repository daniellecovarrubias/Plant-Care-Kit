/*
 * File:   iLED_library.c
 * Author: daniellecovarrubias
 *
 * Created on April 30, 2017, 1:54 PM
 */

#include <p24Fxxxx.h>
#include <xc.h>
#include "stdint.h"
#include "../Project.X/Header_file.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void iLEDsetup(){
    // setup LED pins
    TRISAbits.TRISA2=0; //iLED as output
    TRISAbits.TRISA3 =0; //iLED as output
    LATA=0;
}

void writeColor_suniLED(int r, int g, int b){
    //red
    int i; 
    for (i=0; i<=7; i++){
        if (0b10000000 & r){
            write_1_RA2();
        }
        else {
            write_0_RA2();
        }      
        r = r<<1;
    }
    
    //green
    int j=0;
    for (j=0; j<=7; j++){
        if (0b10000000 & g){
            write_1_RA2();
        }
        else {
            write_0_RA2();
        }    
        
        g = g<<1;
    }
    
    //blue
    int k=0;
    for (k=0; k<=7; k++){
        if (0b10000000 & b){
            write_1_RA2();
        }
        else {
            write_0_RA2();
        }    
        
        b = b<<1;
    }
    
    LATA = 0;
    delay50us();
}

void writeColor_soiliLED(int r, int g, int b){
    //red
    int i; 
    for (i=0; i<=7; i++){
        if (0b10000000 & r){
            write_1_RA3();
        }
        else {
            write_0_RA3();
        }      
        r = r<<1;
    }
    
    //green
    int j=0;
    for (j=0; j<=7; j++){
        if (0b10000000 & g){
            write_1_RA3();
        }
        else {
            write_0_RA3();
        }    
        
        g = g<<1;
    }
    
    //blue
    int k=0;
    for (k=0; k<=7; k++){
        if (0b10000000 & b){
            write_1_RA3();
        }
        else {
            write_0_RA3();
        }    
        
        b = b<<1;
    }
    
    LATA = 0;
    delay50us();
}

void sunDrawFrame(unsigned int frame){
    int red;
    int green;
    green= frame;
    red= 255-frame;
    writeColor_suniLED(red,green,0);
}

void soilDrawFrame(unsigned int frame){
    int red;
    int green;
    green= frame;
    red= 255-frame;
    writeColor_soiliLED(red,green,0);
}

void iLEDsun(int index) {
    int x;
    int green;
    if(index==0){ //UV index of 0 should be a red iLED
        sunDrawFrame(0);
    }
    else if(index >= 10){ //UV index above 10 should be a red iLED
        sunDrawFrame(0);
    }
    else{ //everything in between 0 and 10 should be a gradient of red and green based on the inputted data from the UV sensor
        x=abs(5-index);
        green=((5-x)*255)/5;
        sunDrawFrame(green);
    }
}

void iLEDsoil(int moisture){
    if(moisture < 70 || moisture > 97){ //moisture levels not sufficient or over watered and should be a red iLED
        soilDrawFrame(0);
    }
    else{ //iLED should be a gradient of red and green based on the inputted data from the moisture sensor
        moisture-=70;
        soilDrawFrame(((14-abs(13-moisture))*255)/14);
    }
}