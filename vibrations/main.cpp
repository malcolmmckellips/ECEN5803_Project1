/************************************************************************//**
* \file main.cpp
* \brief Module 2 : Feel the vibrations
*
* Control an RGB LED with accelerometer and touch sensor input to become familiar with mbed
*
****************************************************************************/

#include "mbed.h"
#include "MMA8451Q.h"
#include "TSISensor.h"
 
#define MMA8451_I2C_ADDRESS (0x1d<<1)

Timer t;
float timeSpent;
 
 /**
 * @brief Main function to excersize mbed PwmOut component
 *
 * This function will enable a FRDM board to change the RGB LED color based on accelerometer position. 
 * It will also change the brightness of the LED based on touch sensor input. 
 *
 * 
 */
 
int main(void) {
    MMA8451Q acc(PTE25, PTE24, MMA8451_I2C_ADDRESS);
    PwmOut rled(LED_RED);
    PwmOut gled(LED_GREEN);
    PwmOut bled(LED_BLUE);
    
    TSISensor tsi;
 
    while (true) {
				t.start();
        rled = (1.0 - (abs(acc.getAccX())*(1.0 - tsi.readPercentage())));
        gled = (1.0 - (abs(acc.getAccY())*(1.0 - tsi.readPercentage())));
        bled = (1.0 - (abs(acc.getAccZ())*(1.0 - tsi.readPercentage())));
        t.stop();
				timeSpent = t.read(); //Read time passed in ms
				t.reset();
				wait(0.1);
    }
}