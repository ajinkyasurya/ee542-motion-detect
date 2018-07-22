#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include "PiMotor.h"
#include <wiringPi.h>

#define M1_FWD 	4 //Motor 1 Forward
#define M1_BCK  5 //Motor 1 Reverse
#define M2_FWD  3 //Motor 2 Forward
#define M2_BCK 	0 //Motor 2 Reverse
using namespace std;




int main() {
    
    wiringPiSetup();
	piHiPri(99);
	
	//Create a new instance for our Motor.
    PiMotor motor1(M1_FWD, M1_BCK);
    PiMotor motor2(M2_FWD, M2_BCK);
    
    while (true){
		motor1.setDebug(true); //Turn on Debug message output (see console window!)
		motor1.run(1, 255); //Set PWM value for direction (0 = reverse, 1 = forwards)
		delay(2000);
		motor1.stop(); //Stop the motor  
		
		motor2.setDebug(true); //Turn on Debug message output (see console window!)
		motor2.run(1, 255); //Set PWM value for direction (0 = reverse, 1 = forwards)
		delay(2000);
		motor2.stop(); //Stop the motor  
		
		delay(10000); //10 second delay
    }
   
    return 0;
}

