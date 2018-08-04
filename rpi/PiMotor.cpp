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
    pinMode(pwmPin, OUTPUT);
    softPwmCreate (pwmPin, 0, 100) ; //range = 100
}

void PiMotor::stop() {
    digitalWrite(fwdPin, LOW);
    digitalWrite(revPin, LOW);    
}

void PiMotor::start() {
	softPwmWrite (pwmPin, 30);
    pinMode(fwdPin, OUTPUT);
    pinMode(revPin, OUTPUT);   
}

void PiMotor::runFwd () {
	
    digitalWrite(fwdPin, HIGH);
	digitalWrite(revPin, LOW);
	
}

void PiMotor::runRev () {
	//softPwmWrite (pwmPin, 50);
	digitalWrite(fwdPin, LOW);
    digitalWrite(revPin, HIGH);
}
