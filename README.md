----------------------------------------------
This is a small project to make a Robot Friend
----------------------------------------------

Our friend will try to:
- Be economic -> Have the smallest energy footprint possible.
- Be selfsustained -> Can charge from solar panel.
- Explore -> Move around and don't get stuck in objects.

To achieve this we'll need:
- 2x 3-6v Motors -> Move the wheels so it can explore.
- 1x Low Power Dual Motor Driver(probably DRV3384 from Polou) -> Control the motors.
- 1x Arduino Mini Pro Mini 3.3v/8MHz -> Small energy footprint to control the robot.
- 1x 3-5v Battery, probably a LiPo battery + BMS for 1S -> Provide enough power to keep the robot alive.
- 1x 6V Solar Panel -> Enough to charge the Battery.
- 1x Solar Charger(probably CN3065) -> Convert raw solar power to LiPo standards.
- 2x Distance Sensors(probably VL53L0X and LM393 based IR obstacle detector) -> Look around and don't bump into things.
- 1x IR Receiver -> Receive commands to remotely controll the robot.