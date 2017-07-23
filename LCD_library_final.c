/*
 * File:  
 * Author: Erin
 *
 * Created on February 3, 2017, 6:46 PM
 */
#include <p24Fxxxx.h>
#include <xc.h>
#include <libpic30.h>
#include <stdio.h>
#include "../Project.X/Header_file.h"
#include <string.h>

void delay(int ms){
    for(; ms > 0; ms = ms - 1){
        delay1ms();
    }
}

void delay_us(int us){
    for(; us > 0; us = us - 1){
        wait_1us();
    }
}
void lcd_setup(){
    
    I2C2CONbits.I2CEN = 0;
    I2C2BRG = 157;
    _MI2C2IF = 0;
    I2C2CONbits.I2CEN = 1;
    
    lcd_init();
}
void lcd_init(){
    delay(50);
    lcd_cmd(0b00111000); // function set, normal instruction mode
    delay_us(27);
    lcd_cmd(0b00111001); // function set, extended instruction mode
    delay_us(27);
    lcd_cmd(0b00010100); // interval osc
    delay_us(27);
    lcd_cmd(0b00011110); // contrast C3-C0
    delay_us(27);
    lcd_cmd(0b01011101); // C5-C4, Ion, Bon
    delay_us(27);
    lcd_cmd(0b01101100); // follower control
    delay(201);
    lcd_cmd(0b00111000); // function set, normal instruction mode
    delay_us(27);
    lcd_cmd(0b00001100); // Display On
    delay_us(27);
    lcd_cmd(0b00000001); // Clear Display
    delay(2);
}


void lcd_cmd(char command){
    I2C2CONbits.SEN = 1;
    while(I2C2CONbits.SEN);
    
    _MI2C2IF = 0;
    I2C2TRN = 0b01111100;
    while(!_MI2C2IF);
    
    _MI2C2IF = 0;
    I2C2TRN = 0b00000000;
    while(!_MI2C2IF);
    
    _MI2C2IF = 0;
    I2C2TRN = command;
    while(!_MI2C2IF);
    
    _MI2C2IF = 0;
    I2C2CONbits.PEN = 1;
    while(I2C2CONbits.PEN);
}
void lcd_printChar(char command) {
    I2C2CONbits.SEN = 1;
    while(I2C2CONbits.SEN);
    
    _MI2C2IF = 0;
    I2C2TRN = 0b01111100;
    while(!_MI2C2IF);
    
    _MI2C2IF = 0;
    I2C2TRN = 0b01000000;
    while(!_MI2C2IF);
    
    _MI2C2IF = 0;
    I2C2TRN = command;
    while(!_MI2C2IF);
    
    _MI2C2IF = 0;
    I2C2CONbits.PEN = 1;
    while(I2C2CONbits.PEN);
}
void lcd_setCursor(int x, int y){
    //row y
    //Column x
    char com = (char)((0x40*y + x) + 0b10000000);
    lcd_cmd(com);
}
void lcd_printStr(int start_row, int start_column,const char *s){
    lcd_setCursor(start_column, start_row);
    int i = 0;
    for(; i < strlen(s) ; i++){
        lcd_printChar(s[i]);
    }
}
void lcd_clear(){
    lcd_cmd(0b00000001); // Clear Display
    delay(5);
}
