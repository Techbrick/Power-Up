#include "WPILib.h"
#include "AHRS.h"
#include "ctre/Phoenix.h"
#include <math.h>
#include <thread>
#include <fstream>
#include "Constants.h"
#include "DriveTrain.h"
#include "SuperLifter.h"
#include "PIDLoop.h"

#define PI 3.14159265

#ifndef SRC_ROBOT_H_
#define SRC_ROBOT_H_

class Robot : public SampleRobot {
private:
	DriveTrain robotDrive;
	frc::Joystick driveStick;
	frc::Joystick operatorStick;
	AHRS *gyro;
	Compressor compressor;
	PowerDistributionPanel pdp;
//	SuperLifter lift;
	PIDLoop pid;

public:
	Robot();
	void RobotInit();
	void OperatorControl();
	void Autonomous();
};

#endif
