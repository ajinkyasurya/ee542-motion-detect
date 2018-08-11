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
    int fPin;
    int rPin;
    bool DEBUG = false;
  public:
      PiMotor(int, int);
      void run (int);
      void stop();
      void setDebug(bool); 
};

#endif /* PIMOTOR_H */
