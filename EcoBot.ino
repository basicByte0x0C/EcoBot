/***************************************************************************************
 * Includes
 **************************************************************************************/

/***************************************************************************************
 * Macros
 **************************************************************************************/
/* General stuff */
#define E_OK            0u
#define E_NOT_OK        1u
#define DELAY_DEFAULT   1000

/* Motor Stuff */
#define PIN_DRV8834_M1      D4
#define PIN_DRV8834_SLEEP   D6
#define PIN_MA_PHASE        D7
#define PIN_MA_ENABLE       D3
#define PIN_MB_PHASE        D8
#define PIN_MB_ENABLE       D5

#define DRV8834_MOTOR_A             1u
#define DRV8834_MOTOR_B             2u
#define DRV8834_MOTOR_BOTH          3u
#define DRV8834_DIRECTION_BACKWARD  0u
#define DRV8834_DIRECTION_FORWARD   1u

/***************************************************************************************
 * Notes
 **************************************************************************************/

/* MOTOR DRIVER :: Will use DRV8834 in Phase/Enable Mode with 2x 3-6v DC Motors */
/* BATTERY :: Will use a LiPo battery, for now we'll try to use one old 14500 cell */

/* TODO: Function to set Driver Max Current 
 * - My Motors seems to use 120mA max each */

/* ----- How to controll 2 motors -----
 * - 2 DC Motors can be controlled by DRV8834 with Phase/Enable mode
 * - Each Motor has a ENABLE Pin and a PHASE Pin (xENBL and xPHASE)
 * - xPHASE is controlling the direction, while xENBL controlls the motor
 * - xENBL can be used as PWM to controll the motor
 * - M1 on Low + xENBL on low => Break Mode (0v on both + and -) */

/* TODO: RotateRight(degree) and RotateLeft(degree) functions
 * - Which will move the motors accordingly so the robot can rotate x degrees to the right or left */

/* TODO: Sleep like a baby
 * - Robot will sleep while charging and when the battery level is low
 * - Motor Driver put into sleep mode 
 * - Arduino will sleep and wake up from time to time to check Battery level */

/***************************************************************************************
 * Function: Motor_Break()
 ***************************************************************************************
 * Description: This can be called such that all motors will break and stop moving
 **************************************************************************************/
void Motor_Break(void)
{
    /* To make the motors break in Phase/Enable Mode you need to:
        - M1 pin on Low
        - xENBL on Low
        - xPHASE doesn't matter
       The outputs will be both 0v => Motor will stop */

    /* Disable M0 Pin */
    digitalWrite(PIN_DRV8834_M1, LOW);

    /* Disable Motors */
    analogWrite(PIN_MA_ENABLE, LOW);
    analogWrite(PIN_MB_ENABLE, LOW);
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
     * Let's say that Forward is 1 while backwards is 0 */

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
 * Function: setup()
 ***************************************************************************************
 * Description: This function will setup and initialize things at startup
 **************************************************************************************/
void setup(void)
{
    /* PIN Modes */
    pinMode(PIN_MA_ENABLE, OUTPUT);
    pinMode(PIN_MA_PHASE, OUTPUT);
    pinMode(PIN_MB_ENABLE, OUTPUT);
    pinMode(PIN_MB_PHASE, OUTPUT);

    /* Other */
}

/***************************************************************************************
 * Function: loop()
 ***************************************************************************************
 * Description: This function is executed endlessly after the setup function
 **************************************************************************************/
void loop() 
{
    /* Do nothing for now */
    delay(DELAY_DEFAULT);
}
