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
#define PIN_M1_PHASE    D1
#define PIN_M1_ENABLE   A1
#define PIN_M2_PHASE    D2
#define PIN_M2_ENABLE   A2

/***************************************************************************************
 * Notes
 **************************************************************************************/

/* MOTOR DRIVER :: Will use DRV8834 in Phase/Enable Mode with 2x 3-6v DC Motors */

/* TODO: Function to set Driver Max Current 
 * - My Motors seems to use 120mA max each */

/* TODO: Make functions to controll 2 motors 
 * - 2 DC Motors can be controlled with Phase/Enable mode
 * - Phase is controlling the direction, while Enable controlls the motor
 * - Enable can be used as PWM to controll the motor
 * - M1 on high + Enable on low => Break Mode (0v on both + and -) */

/* TODO: RotateRight(degree) and RotateLeft(degree) functions
 * - Which will move the motors accordingly so the robot can rotate x degrees to the right or left */

/* TODO: Sleep like a baby
 * - Robot will sleep while charging and when the battery level is low
 * - Motor Driver put into sleep mode 
 * - Arduino will sleep and wake up from time to time to check Battery level */

/***************************************************************************************
 * Function: setup
 ***************************************************************************************
 * Description: This function will setup and initialize things at startup
 **************************************************************************************/
void setup(void)
{
    /* PIN Modes */
    pinMode(PIN_M1_ENABLE, OUTPUT);
    pinMode(PIN_M1_PHASE, OUTPUT);
    pinMode(PIN_M2_ENABLE, OUTPUT);
    pinMode(PIN_M2_PHASE, OUTPUT);

    /* Other */
}

/***************************************************************************************
 * Function: loop
 ***************************************************************************************
 * Description: This function is executed endlessly after the setup function
 **************************************************************************************/
void loop() 
{
    /* Do nothing for now */
    delay(DELAY_DEFAULT);
}
