#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "math.h"
#include "spi.h"
#include "font.h"
#include <stdio.h>
#include "main.h"
#include "ST7789.h"

// DEVCFG0
#pragma config DEBUG = OFF // disable debugging
#pragma config JTAGEN = OFF // disable jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // disable flash write protect
#pragma config BWP = OFF // disable boot write protect
#pragma config CP = OFF // disable code protect

// DEVCFG1
#pragma config FNOSC = FRCPLL // use primary oscillator with pll
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
    
    initSPI();
    LCD_init();
    
    unsigned char x = 28, y = 32, X=28,Y=64, xx = 28, yy = 48;
    int i = 0;
    float j;
    unsigned char h = 72;
    LCD_clearScreen(BLACK);
    
    while(1){
        _CP0_SET_COUNT(0);
        Heartbeat();
        char message[100]; 
        char message2[100];
        sprintf(message,"Hello World %d",i);
        drawString(x,y,BLUE,message);
        i++;
        if (i == 100){
            i = 0;
        }
        progressBar(xx,yy,BLUE,i);
        j = (float) 24000000/_CP0_GET_COUNT();
        sprintf(message2,"FPS = %.2f",j);
        drawString(X,Y,BLUE,message2);
    }
}

void delay() {
    _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() < 48000000 / 2 /100  ){} // 10 Hz
}

void Heartbeat() {
    LATAbits.LATA4 = 1;
    delay();
    LATAbits.LATA4 = 0;
    delay();
}

//void bar_x(signed short x){
//    int i = 63;
//    int j = 15;
//    int k;
//    if (x < 0 ){
//        for (k = i;k > x/1000+i;k--){
//            ssd1306_drawPixel(k,j,1);
//        }
//        for (k = i-j;k < x/1000+i;k++){
//            ssd1306_drawPixel(k,j,0);
//        }
//    }      
//    else if (x > 0 ){
//        for (k = i;k < x/1000+i;k++){
//            ssd1306_drawPixel(k,j,1);
//        }
//        for (k = i+j;k > x/1000+i;k--){
//            ssd1306_drawPixel(k,j,0);
//        }
//        }
//}
