/***************************************************************************************
 * Includes
 **************************************************************************************/
#include "Notes.h"
#include "IRremote.h"
#include <LowPower.h>

/***************************************************************************************
 * Macros
 **************************************************************************************/
/* General Stuff */
#define E_OK            0u
#define E_NOT_OK        1u
#define DELAY_1_SECOND  1000
#define DELAY_DEFAULT   DELAY_1_SECOND
#define SERIAL_BRATE    115200
static byte devStuff = E_NOT_OK;
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
#define DRV8834_POWER_FULL          255u    /* Doesn't work with less than max, lower than 3V is not enough for motors or PWM is sketchy */
#define DRV8834_POWER_NONE          0u
#define DRV8834_WAKEUP_WAIT         1       /* Miliseconds until DRV8834 should be fully working after wakeup */
#define DRV8834_WALK_TIME           100
#define DRV8834_BREAK_TIMEOUT       115     /* Lower than this and the timeout is too short */
#define MOTOR_TURN_TIME_FACTOR      4       /* Multiply by degree to turn around. Shoulkd be time for 1 degree */
/* Motor Stuff end */

/* Exploration Stuff */
#define EXPLORE_AUTOMATE        0u    /* Autonomous driving */
#define EXPLORE_MANUAL          1u    /* Manual driving from IR */
#define EXPLORE_ROTATE_45       45
#define EXPLORE_ROTATE_90       90
#define EXPLORE_ROTATE_180      180
#define EXPLORE_ROTATE_CUSTOM   135
#define EXPLORE_ROTATE_DEFAULT  EXPLORE_ROTATE_90
#define EXPLORE_OBSTACLE_NONE   0u
#define EXPLORE_OBSTACLE_LEFT   1u
#define EXPLORE_OBSTACLE_RIGHT  2u
#define EXPLORE_OBSTACLE_BOTH   3u 

static byte exploreState = EXPLORE_MANUAL;
/* Exploration Stuff end */

/* IR Stuff */
#define PIN_IR_RECEIVER_POWER   A0      /* Power the IR Receiver with this pin */
#define PIN_IR_OBSTACLE_DATA    8       /* Read data from IR Obstacle Detector  with this pin */
#define PIN_IR_RECEIVER_DATA    9       /* Read data from IR Receiver with this pin */

#define IR_VALUE_FORWARD        0xFF18E7    /* Move Backward */
#define IR_VALUE_BACKWARD       0xFF4AB5    /* Move Forward */
#define IR_VALUE_LEFT           0xFF10EF    /* Rotate Left */
#define IR_VALUE_RIGHT          0xFF5AA5    /* Rotate Right */
#define IR_VALUE_MODE           0xFF38C7    /* Switch between Manual and Automate Exploring */
#define IR_VALUE_SLEEP          0xFF6897    /* Go to Sleep for a while */

IRrecv irrecv(PIN_IR_RECEIVER_DATA);
decode_results results;
/* IR Stuff end */

/* Power Management Stuff */
#define PIN_BATTERY_LEVEL           A3
#define PIN_INSOMNIA          	    2       /* Used for development purpose to keep the Robot awake */
#define ADC_MAX_VALUE               1023.0
#define ADC_MAX_VOLTAGE             3.3
#define BATTERY_SLEEP_THRESHOLD     3.3     /* Voltage drops by 0.05 V when motors are working */
#define ROBOT_SLEEP_1_SECOND        1
#define ROBOT_SLEEP_10_SECONDS      10
#define ROBOT_SLEEP_30_SECONDS      30
#define ROBOT_SLEEP_1_MINUTE        60
#define ROBOT_SLEEP_10_MINUTES      600
#define ROBOT_SLEEP_30_MINUTES      1800
#define ROBOT_SLEEP_TIME_DEFAULT    ROBOT_SLEEP_1_SECOND
/* Power Management Stuff end */

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
 * Description: This function will power a specified motor
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
 * Function: Motor_RotateRight()
 ***************************************************************************************
 * Description: This function will use motors to rotate right.
 * Parameters:
 *  - degrees[in]   :   Number of degrees to rotate right
 **************************************************************************************/
void Motor_RotateRight(uint16_t degrees)
{
    /* Stop Walking */
    Motor_BreakMotor(DRV8834_MOTOR_BOTH);

    /* Rotate around */
    Motor_SwitchDirection(DRV8834_MOTOR_A, DRV8834_DIRECTION_FORWARD);
    Motor_SwitchDirection(DRV8834_MOTOR_B, DRV8834_DIRECTION_BACKWARD);

    /* Try to rotate slowly */
    do
    {
        Motor_EnableMotor(DRV8834_MOTOR_BOTH, DRV8834_POWER_FULL);
        delay(MOTOR_TURN_TIME_FACTOR);
        Motor_BreakMotor(DRV8834_MOTOR_BOTH);
        degrees--;
    }while(0 < degrees);
}

/***************************************************************************************
 * Function: Motor_RotateLeft()
 ***************************************************************************************
 * Description: This function will use motors to rotate left.
 * Parameters:
 *  - degrees[in]   :   Number of degrees to rotate left
 **************************************************************************************/
void Motor_RotateLeft(uint16_t degrees)
{
    /* Stop Walking */
    Motor_BreakMotor(DRV8834_MOTOR_BOTH);

    /* Rotate around */
    Motor_SwitchDirection(DRV8834_MOTOR_A, DRV8834_DIRECTION_BACKWARD);
    Motor_SwitchDirection(DRV8834_MOTOR_B, DRV8834_DIRECTION_FORWARD);

    /* Try to rotate slowly */
    do
    {
        Motor_EnableMotor(DRV8834_MOTOR_BOTH, DRV8834_POWER_FULL);
        delay(MOTOR_TURN_TIME_FACTOR);
        Motor_BreakMotor(DRV8834_MOTOR_BOTH);
        degrees--;
    }while(0 < degrees);
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

  static byte direction = DRV8834_DIRECTION_FORWARD;

  /* Run the motor for 1s */
  Motor_EnableMotor(motorIdentifier, DRV8834_POWER_FULL);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(DELAY_1_SECOND);

  /* Stop the motor for 1s */
  Motor_EnableMotor(motorIdentifier, DRV8834_POWER_NONE);
  digitalWrite(LED_BUILTIN, LOW);
  delay(DELAY_1_SECOND);

  /* Switch Motor Direction */
  direction = !direction;
  Motor_SwitchDirection(motorIdentifier, direction);
}

/***************************************************************************************
 * Function: Robot_WakeUp()
 ***************************************************************************************
 * Description: This function is executed when the robot wakes up. It will initialize
 *              everything and will make the robot functional.
 **************************************************************************************/
void Robot_WakeUp(void)
{
    /* Initialize things */
    /* PIN Modes */
    pinMode(LED_BUILTIN, OUTPUT);   /* Debug purposes */
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

    /* Dev Stuff */
    if(E_OK == devStuff)
    {
        /* New Day! */
        Serial.println("Good Morning!");
    }
}

/***************************************************************************************
 * Function: Robot_Sleep()
 ***************************************************************************************
 * Description: This function tries to make the Robot Sleep.
 * Parameters:
 *  - sleepTime[in]   :   Number of seconds to go to sleep
 *                              Supported Inputs: 
 *                                  0u - 3600u
 **************************************************************************************/
void Robot_Sleep(uint16_t sleepTime)
{
    /* Check if Robot is able to sleep */
    if(HIGH == digitalRead(PIN_INSOMNIA))
    {
        /* Insomnia is here, can't sleep */
    }
    else
    {
        /* Insomnia is not around, sleep */
        /* Set wakeup conditions */
        /* GPIO External Interrupt Wakeup */
        attachInterrupt(PIN_INSOMNIA, Robot_WakeUp, HIGH);

        /* Ensure All Used Pins are configured to output, then output nothing */
        /* Exception for Insomnia to wake it up and Battery Level as it will be always on */
        pinMode(PIN_IR_RECEIVER_POWER, OUTPUT);
        digitalWrite(PIN_IR_RECEIVER_POWER, LOW);   /* Disable IR Receiver */
        pinMode(PIN_IR_RECEIVER_DATA, OUTPUT);
        digitalWrite(PIN_IR_RECEIVER_DATA, LOW);
        pinMode(PIN_DRV8834_SLEEP, OUTPUT);
        digitalWrite(PIN_DRV8834_SLEEP, LOW);   /* Send Motor Driver to sleep */
        pinMode(PIN_MA_ENABLE, OUTPUT);
        digitalWrite(PIN_MA_ENABLE, LOW);
        pinMode(PIN_MA_PHASE, OUTPUT);
        digitalWrite(PIN_MA_PHASE, LOW);
        pinMode(PIN_MB_ENABLE, OUTPUT);
        digitalWrite(PIN_MB_ENABLE, LOW);
        pinMode(PIN_MB_PHASE, OUTPUT);
        digitalWrite(PIN_MB_PHASE, LOW);
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, LOW);        

        /* Dev Stuff */
        if(E_OK == devStuff)
        {
            /* Say Good night */
            Serial.println("Good night!");

            delay(100);
        }

        /* Go to sleep */
        if(sleepTime & 1u)
        {
            LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
        }
        if(sleepTime & 2u)
        {
            LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
        }
        if(sleepTime & 4u)
        {
            LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
        }

        while(sleepTime & 0xFFF8u)
        {
            sleepTime -= 8;
            LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
        }
    }
}

/***************************************************************************************
 * Function: HandleIR()
 ***************************************************************************************
 * Description: This function checks the IR readings and moves the robot accordingly.
 **************************************************************************************/
void HandleIR(void)
{
    /* Dev Stuff */
    if(E_OK == devStuff)
    {
        Serial.print("IR: ");
        Serial.println(results.value, HEX);
    }
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
        case IR_VALUE_SLEEP:
            /* Sleep for a while */
            Motor_BreakMotor(DRV8834_MOTOR_BOTH);
            Robot_Sleep(ROBOT_SLEEP_1_MINUTE);
            Robot_WakeUp();
            break;
        default:
            /* Do nothing */
            break;
    }
}

/***************************************************************************************
 * Function: Robot_DetectAhead()
 ***************************************************************************************
 * Description: Read sensors so we know if the road is blocked.
 **************************************************************************************/
byte Robot_DetectAhead(void)
{
    /* Return if there is any obstacle ahead */
    if(LOW == digitalRead(PIN_IR_OBSTACLE_DATA))
    {
        /* Obstacle ahead */
        return E_NOT_OK;
    }
    else
    {
        /* No obstacle ahead */
    }

    return E_OK;
}

/***************************************************************************************
 * Function: Robot_LookAround()
 ***************************************************************************************
 * Description: Return a value which correspond to obstacles ahead.
 * return       EXPLORE_OBSTACLE_NONE   == 0 : Left and Right are both free
 *              EXPLORE_OBSTACLE_LEFT   == 1 : Left is blocked
 *              EXPLORE_OBSTACLE_RIGHT  == 2 : Right is blocked
 *              EXPLORE_OBSTACLE_BOTH   == 3 : Both Left and Right are blocked
 **************************************************************************************/
byte Robot_LookAround(void)
{
    /* Return Left and Right Readings */
    byte retVal = EXPLORE_OBSTACLE_NONE;

    /* Look Left */
    Motor_RotateLeft(EXPLORE_ROTATE_DEFAULT);
    Motor_BreakMotor(DRV8834_MOTOR_BOTH);
    delay(DELAY_DEFAULT);
    if(E_NOT_OK == Robot_DetectAhead())
    {
        /* Remove this way */
        retVal += EXPLORE_OBSTACLE_LEFT;
    }

    /* Center back */
    Motor_RotateRight(EXPLORE_ROTATE_DEFAULT);
    delay(DELAY_DEFAULT);

    /* Look Right */
    Motor_RotateRight(EXPLORE_ROTATE_DEFAULT);
    delay(DELAY_DEFAULT);
    if(E_NOT_OK == Robot_DetectAhead())
    {
        /* Remove this way as well */
        retVal += EXPLORE_OBSTACLE_RIGHT;
    }

    /* Return to original position */
    Motor_RotateLeft(EXPLORE_ROTATE_DEFAULT);
    delay(DELAY_DEFAULT);

    /* Dev Stuff */
    if(E_OK == devStuff)
    {
        Serial.print("Around: ");
        Serial.println(retVal, HEX);
    }

    return retVal;
}

/***************************************************************************************
 * Function: Robot_Autopilot()
 ***************************************************************************************
 * Description: This function will use Robot intelligence for driving around.
 **************************************************************************************/
void Robot_Autopilot(void)
{
    /* Each bit represents one way: Left, ahead(or back) and Right <-- MSB to LSM */
    byte whereToGo = 2u;
    uint16_t randomDegrees = 180;

    /* Read Distance Sensors and decide if movement is possible */
    /* - IR Sensor detects on LOW read */
    if(E_NOT_OK == Robot_DetectAhead())
    {
        /* Obstacle Detected Ahead */
        /* Stop */
        Motor_BreakMotor(DRV8834_MOTOR_BOTH);
        delay(DELAY_DEFAULT);

        /* Some Dev Stuff */
        if(E_OK == devStuff)
        {
            Serial.println("I see");
        }

        /* Look around */
        whereToGo = Robot_LookAround();

        /* Decide which way to go */
        /* Ahead is already blocked */
        switch(whereToGo)
        {
            case EXPLORE_OBSTACLE_NONE: 
                /* Only obstacle ahead */
                /* Choose Left or Right at random */
                randomSeed(millis());
                if(E_OK == random(2) % 2)
                {
                    /* Go  Right */
                    Motor_RotateRight(EXPLORE_ROTATE_DEFAULT);
                    delay(DELAY_DEFAULT);

                    /* Dev Stuff */
                    if(E_OK == devStuff)
                    {
                        Serial.println("3 -> Go Right");
                    }
                }
                else
                {
                    /* Go Left */
                    Motor_RotateLeft(EXPLORE_ROTATE_DEFAULT);
                    delay(DELAY_DEFAULT);

                    /* Dev Stuff */
                    if(E_OK == devStuff)
                    {
                        Serial.println("3 -> Go Left");
                    }
                }

                break;
            case EXPLORE_OBSTACLE_BOTH:
                /* Only way is to turn around */
                /* Turn around random degrees */
                randomSeed(millis());
                randomDegrees = (uint16_t)random(360);
                Motor_RotateLeft(randomDegrees);
                delay(DELAY_DEFAULT);

                /* Dev Stuff */
                if(E_OK == devStuff)
                {
                    Serial.println("0 -> Go Random");
                }

                break;
            case EXPLORE_OBSTACLE_RIGHT:
                /* Obstacle in Right side */
                Motor_RotateLeft(EXPLORE_ROTATE_DEFAULT);
                delay(DELAY_DEFAULT);
                
                /* Dev Stuff */
                if(E_OK == devStuff)
                {
                    Serial.println("1 -> Go Left");
                }

                break;
            case EXPLORE_OBSTACLE_LEFT:
                /* Obstacle in Left side */
                Motor_RotateRight(EXPLORE_ROTATE_DEFAULT);
                delay(DELAY_DEFAULT);

                /* Dev Stuff */
                if(E_OK == devStuff)
                {
                    Serial.println("2 -> Go Right");
                }

                break;
            default:
                /* If i panic i do nothing */
                break;
        }
    }
    else
    {
        /* No obstacle ahead */
        /* Walk forward */
        Motor_SwitchDirection(DRV8834_MOTOR_BOTH, DRV8834_DIRECTION_FORWARD);
        Motor_EnableMotor(DRV8834_MOTOR_BOTH, DRV8834_POWER_FULL);
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

    
    /* Check Exploring state */
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
        Robot_Autopilot();
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
 * Function: Robot_PowerManagement()
 ***************************************************************************************
 * Description: This function checks the Energy Consumption and decide what to do.
 **************************************************************************************/
void Robot_PowerManagement(void)
{
    volatile uint16_t batteryLevel;
    volatile float batteryVoltage;

    /* Go to sleep if the battery is discharged to save energy and let Solar recharge it */
    do
    {
        /* Read Battery Level */
        batteryLevel = analogRead(PIN_BATTERY_LEVEL);
        
        /* Battery voltage is double the reading value; Voltage Divider is used with R1 = R2 */
        batteryVoltage = (batteryLevel * (ADC_MAX_VOLTAGE / ADC_MAX_VALUE)) * 2;

        /* Check if robot is tired */
        if(BATTERY_SLEEP_THRESHOLD >= batteryVoltage)
        {
            /* Go to sleep */
            Robot_Sleep(ROBOT_SLEEP_TIME_DEFAULT);

            /* Wakeup */
            Robot_WakeUp();
        }
        else
        {
            /* No need to sleep */
            break;
        }
    }while(BATTERY_SLEEP_THRESHOLD >= batteryVoltage);
}

/***************************************************************************************
 * Function: Robot_Testing()
 ***************************************************************************************
 * Description: Code to test. Used for Development purposes and Dev Builds.
 **************************************************************************************/
void Robot_Testing(void)
{
    static uint32_t batteryReadTimeout = 0;
    static float batteryVoltage = 0;

    /* Read Battery Level once every second */
    if(DELAY_1_SECOND < (millis() - batteryReadTimeout))
    {
        /* Update read timeout */
        batteryReadTimeout = millis();
        
        /* Battery voltage is double the reading value; Voltage Divider is used with R1 = R2 */
        batteryVoltage = 2 * (analogRead(PIN_BATTERY_LEVEL) * (ADC_MAX_VOLTAGE / ADC_MAX_VALUE));

        /* Show battery level on Serial */
        Serial.println(batteryVoltage);
    }

    /* Test if motors stopped working */
    //Motor_TestMotor(DRV8834_MOTOR_BOTH);
}

/***************************************************************************************
 * Function: setup()
 ***************************************************************************************
 * Description: This function will setup and initialize things at startup
 **************************************************************************************/
void setup(void)
{
    /* Dev Build */
    if(E_OK == devStuff)
    {
        /* Prepare Debug */
        Serial.begin(SERIAL_BRATE);
        while(!Serial);
    }
    else
    {
        /* Do nothing */
    }

    /* Initialize everything */
    Robot_WakeUp();
}

/***************************************************************************************
 * Function: loop()
 ***************************************************************************************
 * Description: This function is executed endlessly after the setup function
 **************************************************************************************/
void loop(void) 
{
    /* Dev Build */
    if(E_OK == devStuff)
    {
        /* Dev Stuff */
        Robot_Testing();
    }
    else
    {
        /* Production Stuff */
    }

    /* Battery Management */
    Robot_PowerManagement();

    /* Movement Control */
    Robot_Explore();
}
