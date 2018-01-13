#include "../../Yeet/src/Robot.h"

#include "WPILib.h"

Robot::Robot() :
		frontLeftMotor(Constants::frontLeftDriveChannel),
		rearLeftMotor(Constants::rearLeftDriveChannel),
		frontRightMotor(Constants::frontRightDriveChannel),
		rearRightMotor(Constants::rearRightDriveChannel),
		robotDrive(frontLeftMotor, frontRightMotor),
		driveStick(Constants::driveStickChannel),
		operatorStick(Constants::operatorStickChannel),
		compressor(),
		pdp()
{
	gyro = new AHRS(SPI::Port::kMXP);
	robotDrive.SetExpiration(0.1);
	gyro->ZeroYaw();
	rearLeftMotor.Set(ControlMode::Follower,Constants::frontLeftDriveChannel);
	rearRightMotor.Set(ControlMode::Follower,Constants::frontRightDriveChannel);
	//robotDrive.SetInvertedMotor(RobotDrive::kFrontLeftMotor, false);
	rearLeftMotor.ClearStickyFaults(0); //talons browned out and got a low battery fault so they were compensating - this should even out the power and improve drive (especially strafing)
	rearRightMotor.ClearStickyFaults(0); //talons browned out and got a low battery fault so they were compensating - this should even out the power and improve drive (especially strafing)
	frontLeftMotor.ClearStickyFaults(0); //talons browned out and got a low battery fault so they were compensating - this should even out the power and improve drive (especially strafing)
	frontRightMotor.ClearStickyFaults(0); //talons browned out and got a low battery fault so they were compensating - this should even out the power and improve drive (especially strafing)
}

void Robot::RobotInit()
{
}

void Robot::OperatorControl()
{

	robotDrive.SetSafetyEnabled(false);
	gyro.Reset();
	gyro.ResetDisplacement();

	while(IsOperatorControl() && IsEnabled())
	{

		robotDrive.ArcadeDrive(driveStick.GetRawAxis(2),-1*driveStick.GetRawAxis(1));

		frc::Wait(0.005);

	}

	robotDrive.SetSafetyEnabled(true);

}

void Robot::Autonomous()
{
}

START_ROBOT_CLASS(Robot)
