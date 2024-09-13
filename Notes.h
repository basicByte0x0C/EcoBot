#ifndef NOTES_H
#define NOTES_H
/***************************************************************************************
 * Notes
 **************************************************************************************/
/* MICROCONTROLLER :: Will use one Arduino Pro Mini 3.3v/8MHz */
/* MOTOR DRIVER :: Will use DRV8834 in Phase/Enable Mode with 2x 3-6v DC Motors */
/* BATTERY :: Will use a LiPo battery, for now we'll try to use one old 14500 cell */
/* BMS :: Will use any 1S with as much protections as possible */
/* SOLAR PANEL :: Will use one 6V Panel that fits the robot chasis. */
/* SOLAR CHARGER :: Will use one simple CN3065 Solar Charger */
/* SENSORS :: IR Receiver for Remote Control 
            IR Distance for Obstacle Avoidance 
            Laser ToF for stairs avoidance(it won't fall down the stairs) */

/* ----- Arduino Pro Mini -----
 * - Builtin LED will be removed to reduce Energy Consumption, but after everything works(will be used for debug for now).
 * - Voltage Regulator will not be desoldered because the Arduino will be powered directly by unregulated LiPo Battery.
 *      Also, unregulated voltage on pins will mess up Battery Level Reader as there is no fixed anchor for Voltage.
 * - Arduino will be powered through RAW pin because the voltage is unregulated.
 * - Consume aprox 4.11A when Idle, according to some calculations.
 *        Current Pins Usage
 *  D0  --- Reserved for Serial Rx 
 *  D1  --- Reserved for Serial Tx 
 *  D2  --- Reserved for External Interrupts    (Wakeup from Sleep or something)
 *  D3  --- Used by Motor A Enable
 *  D4  --- Used by Motor A Phase
 *  D5  --- Used by Motor B Enable
 *  D6  --- Used by Motor B Phase
 *  D7  --- Used by DRV8834 Sleep Pin
 *  D8  --- Used by IR Obstacle Data
 *  D9  --- Used by IR Receiver Data
 *  D10 --- Reserved for SPI SS(CS) (possible E-Paper?)
 *  D11 --- Reserved for SPI MOSI   (an E-Paper would be nice)
 *  D12 --- Reserved for SPI MISO   (like, you can draw emotions)
 *  D13 --- Reserved for SPI SCK    (on the E-Paper)
 *  A0  --- Used to power IR Modules
 *  A1  ---
 *  A2  ---
 *  A3  --- Used to read Battery Level
 *  A4  --- Reserved for I2C SDA (the laser will be here)
 *  A5  --- Reserved for I2C SCL (and maybe others, who knows)
 *  A6  ---
 *  A7  ---
 *  RST --- Reserved for Reset
 */

/* ----- DRV8834 -----
 * - CFG pin is wired to GND(maybe not needed?).
 * - M1 pin is wired to GND(we don't really need to use it, and Slow Decay is perfect).
 * - Motors need some amperage, doesn't work powered by TTL Serial Programmer, must use a LiPo Battery or similar.
 * - In documentation it is mentioned that when waking up from Sleep the Driver might take up to 1ms to become functional.
 * - Consume aprox 2.5mA when Idle, according to some measurements.
 */

/* ----- LiPo Battery -----
 * - Used an old LiPo single cell Battery, looks like a 14500.
 * - 1200mA or 2200mA one or the other.
 * - For BMS one small, round, green, unnamed, 1S component was used that is based on 8205A Z1J0802 chip.
 * - For Solar Controller an CN3065 Solar Charger v1.0 was used, MPPT doesn't really make sense vor a 6V Panel.
 * - LiPo connected to BMS -> BMS connected to Solar Charger -> Solar Charger connected to Solar Panel + Robot(Arduino+DRV8834).
 */

/* ----- Energy Management -----
 * - Voltage measured with Voltage Divider. R1 == R2. Resistence used: 120 Ohm probably 1W. 2W would be better.
 * - Pin Read Voltage drops by 0.05 V when Motors are running. Take in consideration for threshold. 
 * -- Measurements ::
 *  == No Modifications; no Loop Code ==
 *      - Startup : jumps to ~5mA, fluctuates to 4 and 6 a little bit, then it jumps to Idle current of 7.03mA
 *      - Idle : 7.03 mA
 *      - Motors Running(with loop code) : ~0.21A == 210 mA with Start Spike to 0.27A == 270 mA
 *      - Only Motor Driver Asleep : 4.51 mA => Motor Driver in Idle consume aprox 2.5 mA
 *      - Only IR Receiver Asleep : 6.68 mA => IR Receiver in Idle consume aprox 0.4 mA
 *      - Robot Sleeping : 1.58 mA
 *  == Battery Voltage Reader
 *      - Pin Read : 3.97 V == Multimeter Reading : 4.01 V => Accuracy up to 0.04 V
 *      - Pin Read : 3.95 V == Multimeter Reading : 3.99 V => Accuracy up to 0.04 V
 *      
 */

/* ----- IR Remote Control -----
 * - To remote controll the Robot i added a IR Receiver
 * - IR Data is handled on one pin and another pin is used to power the IR Receiver.
 * - IR Data Values are defined, such it can be changed to any value
 * - IR Receiver i used is an unknown receiver with 3 pins: VCC, GND and Source. I got it from a Plusivo Starter Kit.
 * - Library used to controll the receiver is from the same Plusive Starter Kit examples.
 * - Robot will listen for a IR Mode Change, when this value is received from IR then the Explore State is changed between 
 *      Autonomous and Manual.
 * - In Manual State, it will listen for IR Commands and execute them.
 * - In Autonomous State it will walk autonomously and avoid obstacles with sensors.
 * - If IR is not resumed after reading it it will be stuck with the same value forever. Resume let it read the next command.
 * - Consume aprox 0.4mA when Idle, according to some measurements.
 */

/* TODO: Don't be blind
 * - An IR Distance Sensor will be used to detect objects ahead.
 * - If an object is detected, the robot will rotate around and try to find another path with no obstacles.
 * - IR Output is LOW when there is an obstacle detected, else is HIGH.
 * - Will be powered from the same pin as IR Remote.
 */

/* TODO: Laser Eyes
 * - A Laser ToF Sensor will be used to measure a diagonal distance ahead.
 * - It will be used to detect stairs going down.
 * - It will be positioned looking down ahead, probably at an angle of 45 or 60 degrees.
 * - If a the measured distance is higher than a configured threshold then this means there is a hole/stairs ahead
 *      => don't move 
 */

/* TODO: Function to set Driver Max Current 
 * - My Motors seems to use 120mA max each.
 * - Something that shall be made on Driver Hardware?
 */

/* ----- How to controll 2 motors -----
 * - 2 DC Motors can be controlled by DRV8834 with Phase/Enable mode.
 * - Phase/Enable mode is set by driving the CFG pin to Low.
 * - I soldered the CFG bridge on the DRV8834, but maybe this is not needed(have to test with another driver).
 * - Each Motor has a ENABLE Pin and a PHASE Pin (xENBL and xPHASE).
 * - xPHASE is controlling the direction, while xENBL controlls the motor.
 * - xENBL can be used as PWM to controll the motor.
 */

/* TODO: RotateRight(degree) and RotateLeft(degree) functions
 * - Which will move the motors accordingly so the robot can rotate x degrees to the right or left .
 */

/* ----- Seel like a baby -----
 * - Robot will sleep while charging and when the battery level is low.
 * - Motor Driver put into sleep mode by setting the SLEEP Pin to LOW.
 * - Every other unessential peripheral is powered down.
 * - Arduino will sleep and wake up from time to time to check Battery level.
 * */
#endif /* NOTES_H */
