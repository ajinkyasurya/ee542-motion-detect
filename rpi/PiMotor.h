/* 
 * File:   PiMotor.h
 * Author: Priyanka Patil
 *
 * Created on 07 July 2018
 */

#ifndef PIMOTOR_H
#define PIMOTOR_H

class PiMotor {
private:
    int fwdPin;
    int revPin;
    int pwmPin;
    int pwmSpeed = 20;
  public:
      PiMotor(int, int, int);
      void run(int, int);
      void stop();
      void start();
};

#endif /* PIMOTOR_H */
