#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "math.h"
#include "i2c_master_noint.h"

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

unsigned char readPin(unsigned char readaddress, unsigned char rgstr, unsigned char writeaddress);
void setPin(unsigned char writeaddress, unsigned char rgstr, unsigned char value);
void LEDHeartbeat();
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
            
    __builtin_enable_interrupts();
    
    unsigned char writeaddress = 0b01000000;
    unsigned char readaddress = 0b01000001;
    
    i2c_master_setup();
    
    unsigned char IODIRA = 0x00;
    unsigned char IODIRB = 0x01;
    
    unsigned char IODIRA_out = 0x00;
    unsigned char IODIRB_in = 0xFF;
    
    unsigned char OLATA = 0x14;
    unsigned char GPIOB = 0x13;
    
    unsigned char on = 0xFF;
    unsigned char off = 0x00;
    
    unsigned char readval;
    
    
    setPin(writeaddress, IODIRA, IODIRA_out);
    setPin(writeaddress, IODIRB, IODIRB_in);
    
    while(1){
        LEDHeartbeat();
        readval = readPin(readaddress, GPIOB, writeaddress);
        if (readval == 0x00){
            setPin(writeaddress, OLATA, on); 
        }
        else{
            setPin(writeaddress, OLATA, off);
        }
    }
}

void delay() {
    _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() < 24000000/10){} //10 Hz
}

void LEDHeartbeat() {
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
