#include "Robot.h"
#include "WPILib.h"

Robot::Robot() :
		frontLeftMotor(Constants::frontLeftDriveChannel),
		rearLeftMotor(Constants::rearLeftDriveChannel),
		frontRightMotor(Constants::frontRightDriveChannel),
		rearRightMotor(Constants::rearRightDriveChannel),
		robotDrive(frontLeftMotor, rearLeftMotor, frontRightMotor, rearRightMotor),
		driveStick(Constants::driveStickChannel),
		operatorStick(Constants::operatorStickChannel),
		gyro(SPI::Port::kMXP, 200),
		compressor(),
		pdp()
{
	gyro.ZeroYaw();
	robotDrive.SetInvertedMotor(RobotDrive::kFrontLeftMotor, false);
	robotDrive.SetInvertedMotor(RobotDrive::kRearLeftMotor, false);
	robotDrive.SetInvertedMotor(RobotDrive::kFrontRightMotor, true);
	robotDrive.SetInvertedMotor(RobotDrive::kRearRightMotor, true);
	rearLeftMotor.ClearStickyFaults(); //talons browned out and got a low battery fault so they were compensating - this should even out the power and improve drive (especially strafing)
	rearRightMotor.ClearStickyFaults(); //talons browned out and got a low battery fault so they were compensating - this should even out the power and improve drive (especially strafing)
	frontLeftMotor.ClearStickyFaults(); //talons browned out and got a low battery fault so they were compensating - this should even out the power and improve drive (especially strafing)
	frontRightMotor.ClearStickyFaults(); //talons browned out and got a low battery fault so they were compensating - this should even out the power and improve drive (especially strafing)
}

void Robot::RobotInit()
{
}

void Robot::OperatorControl()
{
}

void Robot::Autonomous()
{
}

START_ROBOT_CLASS(Robot)
