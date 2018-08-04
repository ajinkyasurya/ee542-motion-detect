 #include <iostream>
#include <wiringPi.h>
#include <unistd.h>
using namespace std;

#define PWM_LED       18          // this is PWM0, Pin 12
#define BUTTON_GPIO   27          // this is GPIO27, Pin 13
bool running = true;              // fade in/out until button pressed
char c = 0;

void buttonPress(void) {          // ISR on button press - not debounced
   cout << "Enter 'x' to Exit" << endl;
   cin.get(c); 
   if (c == 'x'){
        running = false;               // the while() loop should end soon
    }
}

int main() {                             // must be run as root
   wiringPiSetupGpio();                  // use the GPIO numbering
   pinMode(PWM_LED, PWM_OUTPUT);         // the PWM LED - PWM0
   pinMode(BUTTON_GPIO, INPUT);          // the button input
   wiringPiISR(BUTTON_GPIO, INT_EDGE_RISING, &buttonPress);

   cout << "Fading the LED in/out until the button is pressed" << endl;
   while(running) {
      for(int i=1; i<=1023; i++) {       // Fade fully on
         pwmWrite(PWM_LED, i);
         usleep(1000);
      }
      for(int i=1022; i>=0; i--) {       // Fade fully off
         pwmWrite(PWM_LED, i);
         usleep(1000);
      }
   }
   cout << "LED Off: Program has finished gracefully!" << endl;
   return 0;
}
