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

PiMotor::PiMotor(int forwardPin, int reversePin) {    
	//Initialise GPIO.
	if (wiringPiSetup() < 0) {
      return;
	}
    fwdPin = forwardPin;
    revPin = reversePin;
    pinMode(fwdPin, OUTPUT);
    pinMode(revPin, OUTPUT);
}

void PiMotor::stop() {
    digitalWrite(fwdPin, LOW);
    digitalWrite(revPin, LOW);    
}

void PiMotor::start() {
    pinMode(fwdPin, OUTPUT);
    pinMode(revPin, OUTPUT);   
}

void PiMotor::runFwd () {
    digitalWrite(fwdPin, HIGH);
	digitalWrite(revPin, LOW);
}

void PiMotor::runRev () {
	digitalWrite(fwdPin, LOW);
    digitalWrite(revPin, HIGH);
}
