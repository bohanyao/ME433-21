#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "math.h"
#include "i2c_master_noint.h"
#include "font.h"
#include <stdio.h>
#include "imu.h"
#include "main.h"

// DEVCFG0
#pragma config DEBUG = OFF // disable debugging
#pragma config JTAGEN = OFF // disable jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // disable flash write protect
#pragma config BWP = OFF // disable boot write protect
#pragma config CP = OFF // disable code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // disable secondary oscillator
#pragma config IESO = OFF // disable switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable clock output
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // disable clock switch and FSCM
#pragma config WDTPS = PS1048576 // use largest wdt
#pragma config WINDIS = OFF // use non-window mode wdt
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations


int main() {

    __builtin_disable_interrupts(); // disable interrupts while initializing things

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    TRISAbits.TRISA4 = 0;
            
    __builtin_enable_interrupts();
    
    i2c_master_setup();
    ssd1306_setup();
    imu_setup();

    signed short imu[7];
    
    while(1){
        if (1) {
            imu_read(IMU_OUT_TEMP_L, imu,7);
            char message[100]; 
            sprintf(message,"g: %d %d %d ",imu[1],imu[2],imu[3]);
            printChars(0,0,message);
            sprintf(message,"a: %d %d %d ",imu[4],imu[5],imu[6]);
            printChars(0,8,message);
            sprintf(message,"t: %d ",imu[0]);
            printChars(0,16,message);
            ssd1306_update();
        }
        else{
            imu_read(IMU_OUT_TEMP_L, imu,7);
            bar_x(-imu[5]);
            bar_y(imu[4]);
            ssd1306_update();
        }
    }
}

void delay() {
    _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() < 48000000 / 2 /10  ){} // 10 Hz
}

void Heartbeat() {
    LATAbits.LATA4 = 1;
    delay();
    LATAbits.LATA4 = 0;
    delay();
}

void setPin(unsigned char writeaddress, unsigned char rgstr, unsigned char value) {
    i2c_master_start();
    i2c_master_send(writeaddress);
    i2c_master_send(rgstr);
    i2c_master_send(value);
    i2c_master_stop();
}

unsigned char readPin(unsigned char readaddress, unsigned char rgstr, unsigned char writeaddress) {
    int i = 1;
    unsigned char readval;
    i2c_master_start();
    i2c_master_send(writeaddress);
    i2c_master_send(rgstr);
    i2c_master_restart();
    i2c_master_send(readaddress);
    readval = i2c_master_recv();
    i2c_master_ack(i);
    i2c_master_stop();
    
    return(readval);
}


void I2C_read_multiple(unsigned char readaddress, unsigned char writeaddress, unsigned char rgstr, unsigned char * data, int length) {
    int j;
    
    i2c_master_start();
    i2c_master_send(writeaddress);
    i2c_master_send(rgstr);
    i2c_master_restart();
    i2c_master_send(readaddress);
    for (j = 0;j<length;j++){
        data[j] = i2c_master_recv();
        if (j<length-1){
            i2c_master_ack(0);
        }
        else {
            i2c_master_ack(1);
        }
    }
    i2c_master_stop();
}



void drawLetter(unsigned char x, unsigned char y, unsigned char letter){
    unsigned char on = 1;
    unsigned char off = 0;
    int j;
    int k;
    for(j = 0; j <5;j++){
        for(k=0;k<8;k++){
            if((((ASCII[letter - 0x20][j])>>k)&1)==1){
                ssd1306_drawPixel(j+x,k+y,on);
            }
            else
                ssd1306_drawPixel(j+x,k+y,off);
        }
    }
}

void printChars(unsigned char x, unsigned char y, unsigned char *charArray){
    int s = 0;
    while(charArray[s]!='\0' || charArray[s] != 0){
        drawLetter(x,y,charArray[s]);
        x+=5;
        if(x>124){
            x = 0;
            y+=8;
            if(y>31){
                y = 0;
            }
        }
        s++;
    }
}

void bar_x(signed short x){
    int i = 63;
    int j = 15;
    int k;
    if (x < 0 ){
        for (k = i;k > x/1000+i;k--){
            ssd1306_drawPixel(k,j,1);
        }
        for (k = i-j;k < x/1000+i;k++){
            ssd1306_drawPixel(k,j,0);
        }
    }      
    else if (x > 0 ){
        for (k = i;k < x/1000+i;k++){
            ssd1306_drawPixel(k,j,1);
        }
        for (k = i+j;k > x/1000+i;k--){
            ssd1306_drawPixel(k,j,0);
        }
        }
}

void bar_y(signed short y){
    int i = 63;
    int j = 15;
    int k;
    if (y < 0 ){
        for (k = j;k > y/1000+j;k--){
            ssd1306_drawPixel(i,k,1);
        }
        for (k = j-j;k < y/1000+j;k++){
            ssd1306_drawPixel(i,k,0);
        }
    }      
    else if (y > 0 ){
        for (k = j;k < y/1000+j;k++){
            ssd1306_drawPixel(i,k,1);
        }
        for (k = j+j;k > y/1000+j;k--){
            ssd1306_drawPixel(i,k,0);
        }
        }
}