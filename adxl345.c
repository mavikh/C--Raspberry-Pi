/*
 This program sets up an ADXL345 accelerometer on I2C port of a Raspberry-Pi. 
 It prints out the data of 3 axis(x,y,z) of it.
*/
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

//-------- define Adresses:
#define adxl345_add	 0x1d
#define BW_Rate_reg_add  0x2c 
#define PWR_CTL_reg_add  0x2d
#define DATA_FORMAT_reg_add  0x31
#define DATAX0_add  0x32
#define DATAX1_add  0x33
#define DATAY0_add  0x34
#define DATAY1_add  0x35
#define DATAZ0_add  0x36
#define DATAZ1_add  0x37

//-------- define Variables:
int fd;
char X0,X1, Y0,Y1, Z0,Z1;
int X,Y,Z,Xs,x16;
float X11,Y11,Z11;
const float scale_factor_2g = 3.9; //mg/LSB
struct axis{
	int x, y, z;
};

//-------- Function Decleration:
void initialize_register(int fd);
struct axis read_axis();


//-------- Initialize Registers:
void initialize_register(int fd){
	wiringPiI2CWriteReg8(fd,BW_Rate_reg_add,0x09);// 0x09: Normal operation, Output data rate= 50 Hz
	wiringPiI2CWriteReg8(fd,PWR_CTL_reg_add, 0x08);// Auto_sleep disable
	wiringPiI2CWriteReg8(fd,DATA_FORMAT_reg_add, 0x08);// OutputResolution: +/-2g Range(10 bit)
										   // set sensitivity mode of device to 2g --> each LSB = 3.9milli g
}

//-------- Read and Handle Data:
struct axis read_axis(){
	struct axis acc_axis;
	X0= wiringPiI2CReadReg8 (fd, DATAX0_add);
	X1= wiringPiI2CReadReg8 (fd, DATAX1_add);
//	x16= wiringPiI2CReadReg16 (fd, DATAX0_add);

//	printf("X0:%d , X1:%d, x16:%d", X0, X1, x16);
//	printf("\n");
	X = 256*X1 + X0;// join Upper & Lower bytes

//	X11 = 256*X1 + X0;
//	Xs = (int)(X1<<8) + (int)X0;

//	printf("Xs:%d ", Xs);
//	printf("\n");

	if (X > 511) // bit 10 is 1: X is 2's complement of a digit
		X = X-1024; // gives back the real negative value of X-axis
	X = X * scale_factor_2g;

	Y0= wiringPiI2CReadReg8 (fd, DATAY0_add);
	Y1= wiringPiI2CReadReg8 (fd, DATAY1_add);
	Y = 256*Y1 + Y0;
//	Y11 = 256*Y1 + Y0;

	if (Y > 511) // bit 10 is 1: Y is 2's complement of a digit
		Y = Y-1024; // gives back the real negative value of Y-axis	؛
	Y = Y * scale_factor_2g;

	Z0 = wiringPiI2CReadReg8 (fd, DATAZ0_add);
	Z1 = wiringPiI2CReadReg8 (fd, DATAZ1_add);
	Z = 256*Z1 + Z0;
//	Z11 = 256*Z1 + Z0;

	if (Z > 511) // bit 10 is 1: Z is 2's complement of a digit
		Z = Z-1024; // gives back the real negative value of Z-axis
	Z = Z * scale_factor_2g;

	acc_axis.x = X;
	acc_axis.y = Y;
	acc_axis.z = Z;
	return acc_axis;
}

int  main(void){
	struct axis acc_axis;
	fd = wiringPiI2CSetup(adxl345_add);// initializes I2C system;
	if(fd == -1)	perror("I2C Setup Error!");
	initialize_register(fd);
	for(;;){
		acc_axis = read_axis();
//		printf("X:%f Y:%f  Z:%f",X,Y,Z);
		printf("X:%f Y:%f  Z:%f",acc_axis.x,acc_axis.y,acc_axis.z);
		printf("\n");

		//printf("X= %d", X, "Y= %d", Y, "Z= %d",Z);

		//printf("X11:%d Y11:%d  Z11:%d",X11,Y11,Z11);


		//printf("X= %d", X, "Y= %d", Y, "Z= %d",Z);
		//printf("\n");
		//printf( X, Y, Z);
		//printf("\n");
	}

	return 0;
} 
