#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "spi.h"
#include "math.h"

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

void delay();

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
    LATAbits.LATA4 = 0;
    TRISBbits.TRISB4 = 1;
    
    initSPI();
    
    __builtin_enable_interrupts();

    unsigned char channel1 = 0; //A = 0, B = 1
    unsigned char channel2 = 1;
    unsigned short p1;
    unsigned short p2;
    
    unsigned short sinarray[] = {511,575,638,699,758,812,861,906,943,974,998,1014,1022,1022,1014,998,974,943,906,861,812,758,699,638,575,511,447,384,323,264,210,161,116,79,48,24,8,0,0,8,24,48,79,116,161,210,264,323,384,447,511,575,638,699,758,812,861,906,943,974,998,1014,1022,1022,1014,998,974,943,906,861,812,758,699,638,575,511,447,384,323,264,210,161,116,79,48,24,8,0,0,8,24,48,79,116,161,210,264,323,384,447,511};
    unsigned short triarray[] = {0,19,40,60,81,101,122,142,163,183,204,224,245,265,286,306,327,347,368,388,409,429,450,470,491,511,531,552,572,593,613,634,654,675,695,716,736,757,777,798,818,839,859,880,900,921,941,962,982,1003,1023,1003,982,962,941,921,900,880,859,839,818,798,777,757,736,716,695,675,654,634,613,593,572,552,531,511,491,470,450,429,409,388,368,347,327,306,286,265,245,224,204,183,163,142,122,101,81,60,40,19,0};
            
    int j;
    
    while (1) {
        for (j = 0; j<101; j++)
        {
            int sin = sinarray[j];
            int tri = triarray[j];
            p1 = 0b0000000000000000;
            p2 = 0b0000000000000000;
            p1 = channel1 << 15;
            p2 = channel2 << 15;
            p1 = p1|(0b111<<12);
            p2 = p2|(0b111<<12);
            p1 = p1|(sin<<2);
            p2 = p2|(tri<<2);
            LATAbits.LATA0 = 0;
            spi_io(p1>>8);
            spi_io(p1);
            LATAbits.LATA0 = 1;
            LATAbits.LATA0 = 0;
            spi_io(p2>>8);
            spi_io(p2);
            LATAbits.LATA0 = 1;
            delay();
        }
    }
}

void delay() {
    _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() < 48000000 / 200){} // 100 Hz
}

