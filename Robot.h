#include "WPILib.h"
#include "AHRS.h"
#include "ctre/Phoenix.h"
#include <math.h>
#include <thread>
#include <fstream>
#include "../../Yeet/src/Constants.h"

#define PI 3.14159265

#ifndef SRC_ROBOT_H_
#define SRC_ROBOT_H_

class Robot : public SampleRobot {

	TalonSRX frontLeftMotor;
	TalonSRX rearLeftMotor;
	TalonSRX frontRightMotor;
	TalonSRX rearRightMotor;
	frc::RobotDrive robotDrive;
	frc::Joystick driveStick;
	frc::Joystick operatorStick;
	AHRS *gyro;
	Compressor compressor;
	PowerDistributionPanel pdp;

public:
	Robot();
	void RobotInit();
	void OperatorControl();
	void Autonomous();
};

#endif
