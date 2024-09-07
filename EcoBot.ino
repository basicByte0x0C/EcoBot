/***************************************************************************************
 * Includes
 **************************************************************************************/
#include "Notes.h"
#include "IRremote.h"

/***************************************************************************************
 * Macros
 **************************************************************************************/
/* General Stuff */
#define E_OK            0u
#define E_NOT_OK        1u
#define DELAY_1_SECOND  1000
#define DELAY_DEFAULT   DELAY_1_SECOND
#define SERIAL_BRATE    115200
/* General Stuff end */

/* Motor Stuff */
#define PIN_MA_ENABLE         3
#define PIN_MA_PHASE          4
#define PIN_MB_ENABLE         5
#define PIN_MB_PHASE          6
#define PIN_DRV8834_SLEEP     7

#define DRV8834_MOTOR_A             1u
#define DRV8834_MOTOR_B             2u
#define DRV8834_MOTOR_BOTH          3u
#define DRV8834_DIRECTION_BACKWARD  0u
#define DRV8834_DIRECTION_FORWARD   1u
#define DRV8834_POWER_FULL          255u
#define DRV8834_POWER_HALF          128u
#define DRV8834_POWER_NONE          0u
#define DRV8834_WAKEUP_WAIT         1     /* Miliseconds until DRV8834 should be fully working after wakeup */
#define DRV8834_WALK_TIME           100
#define DRV8834_BREAK_TIMEOUT       115   /* Lower than this and the timeout is too short */
/* Motor Stuff end */

/* Exploration Stuff */
#define EXPLORE_AUTOMATE  0u    /* Autonomous driving */
#define EXPLORE_MANUAL    1u    /* Manual driving from IR */

static byte exploreState = EXPLORE_AUTOMATE;
/* Exploration Stuff end */

/* IR Stuff */
#define PIN_IR_RECEIVER_POWER   A0    /* Power the IR Receiver with this pin */
#define PIN_IR_RECEIVER_DATA    9     /* Read data from IR Receiver with this pin */

#define IR_VALUE_FORWARD        0xFF18E7    /* Move Backward */
#define IR_VALUE_BACKWARD       0xFF4AB5    /* Move Forward */
#define IR_VALUE_LEFT           0xFF10EF    /* Rotate Left */
#define IR_VALUE_RIGHT          0xFF5AA5    /* Rotate Right */
#define IR_VALUE_MODE           0xFF38C7    /* Switch between Manual and Automate Exploring */

IRrecv irrecv(PIN_IR_RECEIVER_DATA);
decode_results results;
/* IR Stuff end */

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
     * - xENBL on Low
     * - xPHASE doesn't matter
     * The outputs will be both 0v => Motor will stop */

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
   * and also the switching of the direction shall work. */

  static byte direction = 1u;

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
 * Function: HandleIR()
 ***************************************************************************************
 * Description: This function checks the IR readings and moves the robot accordingly.
 **************************************************************************************/
void HandleIR(void)
{
    /* Robot reaction based on the IR value */
    switch(results.value)
    {
        case IR_VALUE_FORWARD: 
            /* Move Forward */
            Motor_SwitchDirection(DRV8834_MOTOR_BOTH, DRV8834_DIRECTION_FORWARD);
            Motor_EnableMotor(DRV8834_MOTOR_BOTH, DRV8834_POWER_FULL);
            break;
        case IR_VALUE_BACKWARD:
            /* Move Backwards */
            Motor_SwitchDirection(DRV8834_MOTOR_BOTH, DRV8834_DIRECTION_BACKWARD);
            Motor_EnableMotor(DRV8834_MOTOR_BOTH, DRV8834_POWER_FULL);
            break;
        case IR_VALUE_LEFT:
            /* Rotate Left */
            Motor_SwitchDirection(DRV8834_MOTOR_A, DRV8834_DIRECTION_BACKWARD);
            Motor_SwitchDirection(DRV8834_MOTOR_B, DRV8834_DIRECTION_FORWARD);
            Motor_EnableMotor(DRV8834_MOTOR_BOTH, DRV8834_POWER_FULL);
            break;
        case IR_VALUE_RIGHT:
            /* Rotate Right */
            Motor_SwitchDirection(DRV8834_MOTOR_A, DRV8834_DIRECTION_FORWARD);
            Motor_SwitchDirection(DRV8834_MOTOR_B, DRV8834_DIRECTION_BACKWARD);
            Motor_EnableMotor(DRV8834_MOTOR_BOTH, DRV8834_POWER_FULL);
            break;
        case IR_VALUE_MODE:
            /* Change Explore State */
            exploreState = !exploreState;
            Motor_BreakMotor(DRV8834_MOTOR_BOTH);
            break;
        default:
            /* Do nothing */
            break;
    }
}

/***************************************************************************************
 * Function: Robot_Explore()
 ***************************************************************************************
 * Description: This function verify Explore Mode and decide how to control the robot.
 **************************************************************************************/
void Robot_Explore(void)
{
    /* Explore the world based on the active state
     * - Automate = drive autonomously but check IR Receiver for Mode Switch first
     * - Manual = drive based on IR commands */

    /* Timeout to stop the motors
     * - when it is 0 it is disabled
     * - when it has a value it is started and checked upon the threshold */
    static volatile long breakTime = 0;

    
    /* Check Exploreing state */
    if(exploreState == EXPLORE_AUTOMATE)
    {
        /* Try to read IR Receiver */
        if(irrecv.decode(&results))
        {
            /* Resume IR */
            irrecv.resume();

            /* Check for Explore Mode */
            if(IR_VALUE_MODE == results.value)
            {
                /* Switch Explore State */
                exploreState = !exploreState;
                Motor_BreakMotor(DRV8834_MOTOR_BOTH);
                return; /* Skip Autonomous part */
            }
            else
            {
                /* Do Nothing */
            }
        }
        else
        {
            /* No IR Commands */
        }

        /* Do Autonomous things */
        /* Check for Obstacles */
        /* Decide next Direction */
        /* Move */
    }
    else
    {
        /* Do Manual things */
        /* Check for IR Input */
        if(irrecv.decode(&results))
        {
            /* Resume IR */
            irrecv.resume();

            /* Handle IR Input */
            HandleIR();

            /* Start Break timeout */
            breakTime = millis();
        }
        else
        {
            /* Stop motors if some time passed since last command */
            if((0 != breakTime) && (DRV8834_BREAK_TIMEOUT < (millis() - breakTime)))
            {
                /* Stop Motors */
                Motor_BreakMotor(DRV8834_MOTOR_BOTH);

                /* Disable Break Timeout */
                breakTime = 0;
            }
            else
            {
                /* Do Nothing */
            }
        }
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
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PIN_MA_ENABLE, OUTPUT);
    pinMode(PIN_MA_PHASE, OUTPUT);
    pinMode(PIN_MB_ENABLE, OUTPUT);
    pinMode(PIN_MB_PHASE, OUTPUT);
    pinMode(PIN_DRV8834_SLEEP, OUTPUT);
    pinMode(PIN_IR_RECEIVER_POWER, OUTPUT);
    pinMode(PIN_IR_RECEIVER_DATA, INPUT);

    /* Wakeup the Motor Driver */
    digitalWrite(PIN_DRV8834_SLEEP, HIGH);
    delay(DRV8834_WAKEUP_WAIT);

    /* Set default direction of both Motors to move Forward */
    Motor_SwitchDirection(DRV8834_MOTOR_BOTH, DRV8834_DIRECTION_FORWARD);

    /* Power on the IR Receiver */
    digitalWrite(PIN_IR_RECEIVER_POWER, HIGH);

    /* Enable IR Receiver */
    irrecv.enableIRIn();
}

/***************************************************************************************
 * Function: loop()
 ***************************************************************************************
 * Description: This function is executed endlessly after the setup function
 **************************************************************************************/
void loop(void) 
{
    /* Battery Management */
    //TODO();

    /* Movement Control */
    Robot_Explore();
}
