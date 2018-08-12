#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <wiringPi.h>
#include "PiMotor.h"
#include "tof.h"

#define M1_FWD 	25 //Motor 1 Forward
#define M1_BCK  24 //Motor 1 Reverse
#define M2_FWD  28 //Motor 2 Forward
#define M2_BCK 	27 //Motor 2 Reverse
#define M1_PWM  1 //Motor 1 pwm
#define M2_PWM  29 //Motor 2 pwm 
#define BLE 23
#define MINIMUM_SPEED 20
#define MAXIMUM_SPEED 100
using namespace std;

//Create a new instance for our Motor.

int distReading;
int direction;
int speed = 20;

//bluetooth variables
struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
char buf[1024] = { 0 };
int s, client, bytes_read;
socklen_t opt = sizeof(rem_addr);

int ble() {
	// allocate socket
	s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	// bind socket to port 1 of the first available 
	// local bluetooth adapter
	loc_addr.rc_family = AF_BLUETOOTH;
	bdaddr_t my_bdaddr_any = { 0 };
	loc_addr.rc_bdaddr = my_bdaddr_any;
	loc_addr.rc_channel = (uint8_t)1;
	bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

	// put socket into listening mode
	listen(s, 1);

	// accept one connection
	client = accept(s, (struct sockaddr *)&rem_addr, &opt);

	ba2str(&rem_addr.rc_bdaddr, buf);
	fprintf(stderr, "Accepted connection from %s\n", buf);
	memset(buf, 0, sizeof(buf));
	return 0;
}

void setDistaceSensor() {
	int i;
	int model, revision;
	// For Raspberry Pi's, the I2C channel is usually 1
	i = tofInit(1, 0x29, 1); // set long range mode (up to 2m)
	if (i != 1)
	{
		printf("VL53L0X device not opened.\n");
		return; // -1; // problem - quit
	}
	printf("VL53L0X device successfully opened.\n");
	i = tofGetModel(&model, &revision);
	printf("Model ID - %d\n", model);
	printf("Revision ID - %d\n", revision);
}

int getDistance() {
		int dist, threshold = 99999;

		dist = tofReadDistance();
		// valid range? 
		if (dist < 4096){
			return dist;
		} else {
			return threshold;
		}
}

void goRun(){

}

void goLeft(){

}

void goRight(){
}

int main(int argc, char *argv[]) {
    
    wiringPiSetup();
	piHiPri(99);
	int bleInd = ble();
	pinMode (BLE, OUTPUT);
	digitalWrite (BLE,  LOW);
	if (bleInd > -1){
		digitalWrite (BLE,  HIGH);
	}
	setDistaceSensor();
	PiMotor motor1(M1_FWD, M1_BCK, M1_PWM);
	PiMotor motor2(M2_FWD, M2_BCK, M2_PWM);

	
	while (true){
		//read data from the client
		bytes_read = read(client, buf, sizeof(buf));
		distReading = getDistance();
		
		if (bytes_read > 0) {
			printf("Received [%s]\n", buf);
		}		
		if ((string)buf == "1"){
			printf("Start\n");			
			printf("Distance: %dmm", distReading );
			//Start the motor
			motor1.start();
			motor2.start();		
		}		
		if ((string)buf == "8"){
			direction = 1;
			printf("Going forward\n");
			printf("Distance: %dmm\n", distReading );
			//Set PWM value for direction (0 = reverse, 1 = forwards)
			motor1.run(direction, speed); 
			motor2.run(direction, speed);
		}		
		if ((string)buf == "2"){
			direction = 0;
			printf("Going reverse\n");
			printf("Distance: %dmm\n", distReading );
			//Set PWM value for direction (0 = reverse, 1 = forwards)
			motor1.run(direction, speed); 
			motor2.run(direction, speed);
		}		
		if ((string)buf == "4"){
			direction = 1;
			printf("Going left\n");
			printf("Distance: %dmm\n", distReading );
			motor1.run(direction, speed); 
			motor2.stop();
		}		
		if ((string)buf == "6"){
			direction = 1;
			printf("Going right\n");
			printf("Distance: %d mm\n", distReading );
			motor2.run(direction, speed); 
			motor1.stop();
		}		
		if ((string)buf == "0"){
			printf("Stop\n");
			//Stop the motor  
			motor1.stop();
			motor2.stop();
		}
		if ((string)buf == "7"){
			printf("Increasing speed\n");
			speed = max(speed + 10, MAXIMUM_SPEED);
			motor1.run(direction, speed); 
			motor2.run(direction, speed);
		}
		if ((string)buf == "9"){
			printf("Decreasing speed\n");
			speed = min(speed - 10, MINIMUM_SPEED);
			motor1.run(direction, speed); 
			motor2.run(direction, speed);
		}
		if ((string)buf == "e"){
			printf("Exit");
			close(client);
			close(s);
			digitalWrite (BLE,  LOW);
			break;
		}
	}
   
	//close connection
	close(client);
	close(s);
	return 0;
}

