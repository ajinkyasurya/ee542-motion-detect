/* 
 * File:   PiMotor.cpp
 * Author: Priyanka Patil
 *
 * Created on 07 July 2017, 17:13
 */
#include <stdio.h>
#include <pigpio.h>
#include <unistd.h>
#include <wiringPi.h>
#include "PiMotor.h"
#include <softPwm.h>

PiMotor::PiMotor(int forwardPin, int reversePin, int PwmPin) {    
	//Initialise GPIO.
	if (wiringPiSetup() < 0) {
      return;
	}
    fwdPin = forwardPin;
    revPin = reversePin;
    pwmPin = PwmPin;
    pinMode(fwdPin, OUTPUT);
    pinMode(revPin, OUTPUT);
    pwmSpeed = 20;
    if (pwmPin == 1){
		pinMode(pwmPin, PWM_OUTPUT);
		pwmSetMode(PWM_MODE_MS);
		pwmSetRange(100);
		pwmSetClock(1920);
		
	} else {
		pinMode(pwmPin, OUTPUT);
		digitalWrite(pwmPin, LOW);
		softPwmCreate (pwmPin, 0, 100) ; //range = 100
	} 
      
}

void PiMotor::stop() {
   if (pwmPin == 1){
		pwmWrite(pwmPin, 0);		
	} else {
		softPwmWrite (pwmPin, 0);
	}	
    digitalWrite(fwdPin, LOW);
    digitalWrite(revPin, LOW);    
}

void PiMotor::start() {
	if (pwmPin == 1){
		pwmWrite(pwmPin, 50);		
	} else {
		softPwmWrite (pwmPin, 50);
	}	
	softPwmWrite (pwmPin, 50);
    pinMode(fwdPin, OUTPUT);
    pinMode(revPin, OUTPUT);   
}

void PiMotor::run(int dir, int speed) {
	pwmSpeed = speed;
	if (dir == 0) {			
		if (pwmPin == 1){
			pwmWrite(pwmPin, pwmSpeed);		
		} else {
			softPwmWrite (pwmPin, pwmSpeed);
		}
		//softPwmWrite(revPin, pwmSpeed);
		digitalWrite(fwdPin, LOW);
		digitalWrite(revPin, HIGH);
	} else if (dir == 1) {
		if (pwmPin == 1){
			pwmWrite(pwmPin, pwmSpeed);		
		} else {
			softPwmWrite (pwmPin, pwmSpeed);
		}		
		//softPwmWrite(fwdPin, pwmSpeed);
		digitalWrite(fwdPin, HIGH);
		digitalWrite(revPin, LOW);
	} else {
	  
		fprintf(stderr, "Invalid Direction Value in PiMotor::run\n\r");
	  return;
	}
}
