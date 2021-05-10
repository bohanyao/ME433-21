#ifndef MAIN_H__
#define MAIN_H__


unsigned char readPin(unsigned char readaddress, unsigned char rgstr, unsigned char writeaddress);
void setPin(unsigned char writeaddress, unsigned char rgstr, unsigned char value);
void Heartbeat();
void delay();

//void bar_x(signed short);

#endif