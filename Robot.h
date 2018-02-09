#include "WPILib.h"
#include "AHRS.h"
#include "ctre/Phoenix.h"
#include <math.h>
#include <thread>
#include <fstream>
#include "Constants.h"
#include "DriveTrain.h"
#include "SuperLifter.h"
#include "SuperClimber.h"
#include "Gripper.h"
#include "PIDLoop.h"
#include "Aimer.h"
#include <pathfinder.h>
#include <iostream>
#include "I2C.h"

#ifndef SRC_ROBOT_H_
#define SRC_ROBOT_H_

class Robot : public SampleRobot {
private:
	AHRS gyro;
	PIDLoop pid;
	DriveTrain robotDrive;
	frc::Joystick driveStick;
	frc::Joystick operatorStick;
	Compressor compressor;
	PowerDistributionPanel pdp;
	SuperLifter lift;
	SuperClimber climber;
	Gripper grip;
	Aimer aim;
	I2C i2c;
	void runPathFinder(Waypoint* points, int POINT_LENGTH);
	void colorSend(uint8_t numToSend);

public:
	Robot();
	void RobotInit();
	void OperatorControl();
	void Autonomous();
};

#endif
