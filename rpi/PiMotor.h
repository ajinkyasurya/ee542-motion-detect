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
  public:
      PiMotor(int, int);
      void runFwd();
      void runRev();
      void stop();
      void start();
};

#endif /* PIMOTOR_H */
