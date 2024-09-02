/***************************************************************************************
 * Includes
 **************************************************************************************/
#include "Notes.h"

/***************************************************************************************
 * Macros
 **************************************************************************************/
/* General Stuff */
#define E_OK            0u
#define E_NOT_OK        1u
#define DELAY_1_SECOND  1000
#define DELAY_DEFAULT   DELAY_1_SECOND
/* General Stuff end */

/* Motor Stuff */
#define PIN_MA_ENABLE       3
#define PIN_MA_PHASE        4
#define PIN_MB_ENABLE       5
#define PIN_MB_PHASE        6
#define PIN_DRV8834_SLEEP   7

#define DRV8834_MOTOR_A             1u
#define DRV8834_MOTOR_B             2u
#define DRV8834_MOTOR_BOTH          3u
#define DRV8834_DIRECTION_BACKWARD  0u
#define DRV8834_DIRECTION_FORWARD   1u
/* Motor Stuff end */

/***************************************************************************************
 * Function: Motor_Break()
 ***************************************************************************************
 * Description: This can be called such that a motor will break and stop moving.
 * Parameters:
 *  - motorIdentifier[in]   :   Identifier of the motor to be switched forward
 *                              Supported Inputs: 
 *                                  DRV8834_MOTOR_A == 1u
 *                                  DRV8834_MOTOR_B == 2u
 *                                  DRV8834_MOTOR_BOTH == 3u
 **************************************************************************************/
void Motor_BreakMotor(byte motorIdentifier)
{
    /* To make the motors break in Phase/Enable Mode you need to:
        - xENBL on Low
        - xPHASE doesn't matter
       The outputs will be both 0v => Motor will stop */

    /* Check which motor to break */
    switch(motorIdentifier)
    {
      case DRV8834_MOTOR_A:
        /* Break Motor A */
        analogWrite(PIN_MA_ENABLE, LOW);
        break;
      case DRV8834_MOTOR_B:
        /* Break Motor B */
        analogWrite(PIN_MB_ENABLE, LOW);
        break;
      case DRV8834_MOTOR_BOTH:
        /* Break Both Motors */
        analogWrite(PIN_MA_ENABLE, LOW);
        analogWrite(PIN_MB_ENABLE, LOW);
        break;
      default:
        /* Motor not recognized */
        break;
    }
}

/***************************************************************************************
 * Function: Motor_SwitchDirection()
 ***************************************************************************************
 * Description: This function will switch the direction of a specified motor
 * Parameters:
 *  - motorIdentifier[in]   :   Identifier of the motor to be switched forward
 *                              Supported Inputs: 
 *                                  DRV8834_MOTOR_A == 1u
 *                                  DRV8834_MOTOR_B == 2u
 *                                  DRV8834_MOTOR_BOTH == 3u
 *  - motorDirection[in]    :   Identifier for the direction to move
 *                              Supported Inputs:
 *                                  DRV8834_DIRECTION_BACKWARD == 0u
 *                                  DRV8834_DIRECTION_FORWARD != 0u
 **************************************************************************************/
void Motor_SwitchDirection(byte motorIdentifier, byte motorDirection)
{
    /* To switch the rotating direction you must se the xPHASE accordingly
     * Forward is 1u while backwards is 0u */

    /* Ensure Direction is valid */
    if(LOW != motorDirection)
    {
        motorDirection = HIGH;
    }

    /* Check which motor to switch */
    switch(motorIdentifier)
    {
        case DRV8834_MOTOR_A:
            /* Change Motor A direction to move Forward */
            digitalWrite(PIN_MA_PHASE, motorDirection);
            break;
        case DRV8834_MOTOR_B:
            /* Change Motor B direction to move Forward */
            digitalWrite(PIN_MB_PHASE, motorDirection);
            break;
        case DRV8834_MOTOR_BOTH:
            /* Change Both Motors direction to move Forward */
            digitalWrite(PIN_MA_PHASE, motorDirection);
            digitalWrite(PIN_MB_PHASE, motorDirection);
            break;
        default:
            /* Motor not recognized */
            break;
    }
}

/***************************************************************************************
 * Function: Motor_EnableMotor()
 ***************************************************************************************
 * Description: This function will switch the direction of a specified motor
 * Parameters:
 *  - motorIdentifier[in]   :   Identifier of the motor to be switched forward
 *                              Supported Inputs: 
 *                                  DRV8834_MOTOR_A == 1u
 *                                  DRV8834_MOTOR_B == 2u
 *                                  DRV8834_MOTOR_BOTH == 3u
 *  - motorPower[in]    :       Value for Motor Power which will be used to enable PWM
 *                              Supported Inputs: 
 *                                  0u - 255u
 **************************************************************************************/
void Motor_EnableMotor(byte motorIdentifier, byte motorPower)
{
    /* A Motor will be enabled through PWM on xENBL pin 
     * The motor can be disabled using value 0 for motorPower */

    /* Check which motor to enable */
    switch(motorIdentifier)
    {
        case DRV8834_MOTOR_A:
            /* Enable Motor A */
            analogWrite(PIN_MA_ENABLE, motorPower);
            break;
        case DRV8834_MOTOR_B:
            /* Enable Motor B */
            analogWrite(PIN_MB_ENABLE, motorPower);
            break;
        case DRV8834_MOTOR_BOTH:
            /* Enable Both Motors */
            analogWrite(PIN_MA_ENABLE, motorPower);
            analogWrite(PIN_MB_ENABLE, motorPower);
            break;
        default:
            /* Motor not recognized */
            break;
    }
}

/***************************************************************************************
 * Function: Motor_TestMotor()
 ***************************************************************************************
 * Description: This can be called to test a motor. The motor shall run for 1s, wait for
 *              another 1s and then change direction.
 * Parameters:
 *  - motorIdentifier[in]   :   Identifier of the motor to be switched forward
 *                              Supported Inputs: 
 *                                  DRV8834_MOTOR_A == 1u
 *                                  DRV8834_MOTOR_B == 2u
 *                                  DRV8834_MOTOR_BOTH == 3u
 **************************************************************************************/
void Motor_TestMotor(byte motorIdentifier)
{
  /* To test the motor functionality, the motor shall run in each direction
    and also the switching of the direction shall work. */

  static byte direction = 0u;

  /* Run the motor for 1s */
  Motor_EnableMotor(motorIdentifier, 255u);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(DELAY_1_SECOND);

  /* Stop the motor for 1s */
  Motor_EnableMotor(motorIdentifier, 0u);
  digitalWrite(LED_BUILTIN, LOW);
  delay(DELAY_1_SECOND);

  /* Switch Motor Direction */
  Motor_SwitchDirection(motorIdentifier, !direction);
  direction = !direction;
}

/***************************************************************************************
 * Function: setup()
 ***************************************************************************************
 * Description: This function will setup and initialize things at startup
 **************************************************************************************/
void setup(void)
{
    /* PIN Modes */
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PIN_MA_ENABLE, OUTPUT);
    pinMode(PIN_MA_PHASE, OUTPUT);
    pinMode(PIN_MB_ENABLE, OUTPUT);
    pinMode(PIN_MB_PHASE, OUTPUT);
    pinMode(PIN_DRV8834_SLEEP, OUTPUT);

    /* Wakeup the Motor Driver */
    digitalWrite(PIN_DRV8834_SLEEP, HIGH);

    /* Set default direction of both Motors to move Forward */
    Motor_SwitchDirection(DRV8834_MOTOR_BOTH, DRV8834_DIRECTION_FORWARD);
}

/***************************************************************************************
 * Function: loop()
 ***************************************************************************************
 * Description: This function is executed endlessly after the setup function
 **************************************************************************************/
void loop(void) 
{
    /* Test the motors */
    Motor_TestMotor(DRV8834_MOTOR_BOTH);
}
