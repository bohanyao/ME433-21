#include "imu.h"

void imu_setup(){
    unsigned char who = 0;
    unsigned char readaddress = 0b11010111;
    unsigned char writeaddress = 0b11010110;

    // read from IMU_WHOAMI
    who = readPin(readaddress, IMU_WHOAMI, writeaddress);
    if (who != 0b01101001){
        while(1){}
    }

    // init IMU_CTRL1_XL
    setPin(writeaddress, IMU_CTRL1_XL, 0b10000010);
    // init IMU_CTRL2_G
    setPin(writeaddress, IMU_CTRL2_G, 0b10001000);
    // init IMU_CTRL3_C
    setPin(writeaddress, IMU_CTRL3_C, 0b00000100);
}

void imu_read(unsigned char reg, signed short * data, int len){
    unsigned char readaddress = 0b11010111;
    unsigned char writeaddress = 0b11010110;
    unsigned char readdata[14];
    int i;
    // read multiple from the imu, each data takes 2 reads so you need len*2 chars
    I2C_read_multiple(readaddress,writeaddress,reg,readdata,len*2);
//    data[0] = readPin(readaddress,reg,writeaddress);
    // turn the chars into the shorts
    for (i = 0;i<len;i++){
        data[i] = (readdata[2*i+1] << 8) | (readdata[2*i]);
    }
}