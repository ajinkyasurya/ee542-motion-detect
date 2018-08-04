#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include "PiMotor.h"
#include <wiringPi.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#define M1_FWD 	4 //Motor 1 Forward
#define M1_BCK  5 //Motor 1 Reverse
#define M2_FWD  3 //Motor 2 Forward
#define M2_BCK 	2 //Motor 2 Reverse
#define M1_PWM  0 //Motor 1 pwm
#define M2_PWM  7 //Motor 2 pwm  -- can be either 1  or 7
using namespace std;

int main() {
    
    wiringPiSetup();
	piHiPri(99);
	
	struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
	char buf[1024] = { 0 };
	int s, client, bytes_read;
	socklen_t opt = sizeof(rem_addr);
	
	//Create a new instance for our Motor.
	PiMotor motor1(M1_FWD, M1_BCK, M1_PWM);
	PiMotor motor2(M2_FWD, M2_BCK, M2_PWM);

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
	
	while (true){
		//read data from the client
		bytes_read = read(client, buf, sizeof(buf));
		//char bufChar = buf[0];
		if (bytes_read > 0) {
			printf("Received [%s]\n", buf);
		}		
		if ((string)buf == "1"){
			printf("Start\n");
			//Start the motor
			motor1.start();
			motor2.start();		
		}		
		if ((string)buf == "8"){
			printf("Going forward\n");
			//Set PWM value for direction (0 = reverse, 1 = forwards)
			motor1.runFwd(); 
			motor2.runFwd();
			
		}		
		if ((string)buf == "2"){
			printf("Going reverse\n");
			//Set PWM value for direction (0 = reverse, 1 = forwards)
			motor1.runRev(); 
			motor2.runRev();
		}		
		if ((string)buf == "4"){
			printf("Going left\n");
			motor1.runFwd(); 
			motor2.stop();
			
		}		
		if ((string)buf == "6"){
			printf("Going right\n");
			motor2.runFwd(); 
			motor1.stop();
		}		
		if ((string)buf == "0"){
			printf("Stop\n");
			//Stop the motor  
			motor1.stop();
			motor2.stop();
		}		
		if ((string)buf == "e"){
			printf("Exit");
			close(client);
			close(s);
			break;
		}
	}
   
	//close connection
	close(client);
	close(s);
	return 0;
}

