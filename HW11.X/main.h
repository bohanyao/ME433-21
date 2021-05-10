#ifndef MAIN_H__
#define MAIN_H__


unsigned char readPin(unsigned char readaddress, unsigned char rgstr, unsigned char writeaddress);
void setPin(unsigned char writeaddress, unsigned char rgstr, unsigned char value);
void Heartbeat();
void delay();

void drawLetter(unsigned char x, unsigned char y, unsigned char letter);
void printChars(unsigned char x, unsigned char y, unsigned char *charArray);

void I2C_read_multiple(unsigned char readaddress, unsigned char writeaddress, unsigned char rgstr, unsigned char * data, int length);

void bar_x(signed short);
void bar_y(signed short);

#endif