#include <DRV8834.h>

#define MOTOR_STEPS 200
#define PIN_DIR   14
#define PIN_STEP  15
#define PIN_M0    25
#define PIN_M1    26

DRV8834 stepper(MOTOR_STEPS, PIN_DIR, PIN_STEP, PIN_M0, PIN_M1);

/*
 * The easy way, albeit not best, is just tell the motor to rotate 360 degrees at 1rpm
 */

void setup() {
    stepper.setRPM(1); 
    stepper.setMicrostep(1); // make sure we are in full speed mode
}

void loop() {
    stepper.rotate(360);
}
