/***************************************************************************************
 * Notes
 **************************************************************************************/
/* MICROCONTROLLER :: Will use one Arduino Pro Mini 3.3v/8MHz */
/* MOTOR DRIVER :: Will use DRV8834 in Phase/Enable Mode with 2x 3-6v DC Motors */
/* BATTERY :: Will use a LiPo battery, for now we'll try to use one old 14500 cell */
/* BMS :: Will use any 1S with as much protections as possible */
/* SOLAR PANEL :: Will use one 6V Panel that fits the robot chasis. */
/* SOLAR CHARGER :: Will use one simple CN3065 Solar Charger */
/* SENSOR :: Will use one HC-SR04 sensor to be aware of the surroundings */

/* ----- Arduino Pro Mini -----
 * - Builtin LED will be removed to reduce Energy Consumption, but after everything works(will be used for debug for now).
 * - Voltage Regulator will not be desoldered because the Arduino will be powered directly by unregulated LiPo Battery.
 * - Arduino will be powered through RAW pin because the voltage is unregulated.
 *        Current Pins Usage
 *  D0  --- Reserved for Serial Rx 
 *  D1  --- Reserved for Serial Tx 
 *  D2  --- Reserved for External Interrupts (Wakeup from Sleep or something)
 *  D3  --- Used by Motor A Enable
 *  D4  --- Used by Motor A Phase
 *  D5  --- Used by Motor B Enable
 *  D6  --- Used by Motor B Phase
 *  D7  --- Used by DRV8834 Sleep Pin
 *  D8  --- (Future) Used by HC-SR04 Trig Pin
 *  D9  --- (Future) Used by HC-SR04 Echo Pin
 *  D10 --- Reserved for SPI SS(CS)
 *  D11 --- Reserved for SPI MOSI
 *  D12 --- Reserved for SPI MISO
 *  D13 --- Reserved for SPI SCK
 *  A0  --- 
 *  A1  ---
 *  A2  ---
 *  A3  ---
 *  A4  --- Reserved for I2C SDA
 *  A5  --- Reserved for I2C SCL
 *  A6  ---
 *  A7  ---
 *  RST --- Reserved for Reset
 */

/* ----- DRV8834 -----
 * - CFG pin is wired to GND(maybe not needed?).
 * - M1 pin is wired to GND(we don't really need to use it, and Slow Decay is perfect).
 * - Motors need some amperage, doesn't work powered by TTL Serial Programmer, must use a LiPo Battery or similar.
 */

/* ----- LiPo Battery -----
 * - Used an old LiPo single cell Battery, looks like a 14500.
 * - 1200mA or 2200mA one or the other.
 * - For BMS one small, round, green, unnamed, 1S component was used that is based on 8205A Z1J0802 chip.
 * - For Solar Controller an CN3065 Solar Charger v1.0 was used, MPPT doesn't really make sense vor a 6V Panel.
 * - LiPo connected to BMS -> BMS connected to Solar Charger -> Solar Charger connected to Solar Panel + Robot(Arduino+DRV8834).
 */

/* TODO: Function to set Driver Max Current 
 * - My Motors seems to use 120mA max each.
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

/* TODO: Sleep like a baby
 * - Robot will sleep while charging and when the battery level is low(another TODO: Read battery level).
 * - Motor Driver put into sleep mode by setting the SLEEP Pin to LOW.
 * - Arduino will sleep and wake up from time to time to check Battery level.
 * */
