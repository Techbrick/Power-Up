#include "Robot.h"

#include "WPILib.h"

Robot::Robot() :
		robotDrive(Constants::frontLeftDriveChannel,Constants::frontRightDriveChannel,Constants::rearLeftDriveChannel,Constants::rearRightDriveChannel),
		driveStick(Constants::driveStickChannel),
		operatorStick(Constants::operatorStickChannel),
		compressor(),
		pdp(),
//		lift(Constants::liftChannel),
		pid()
{
	gyro = new AHRS(SPI::Port::kMXP);
	gyro->ZeroYaw();
}

void Robot::RobotInit()
{
}

void Robot::OperatorControl()
{

	gyro->Reset();
	SmartDashboard::PutBoolean("in op ctrl", true);

	TalonSRX *lift = new TalonSRX(Constants::liftChannel);
	float liftVal = 0;


	while(IsOperatorControl() && IsEnabled())
	{

		robotDrive.TankDrive(0.5*driveStick.GetRawAxis(Constants::tankDriveLeftAxis), 0.5*driveStick.GetRawAxis(Constants::tankDriveRightAxis));

		frc::Wait(0.005);

		SmartDashboard::PutNumber("drive axis 1", driveStick.GetRawAxis(Constants::tankDriveLeftAxis));
		SmartDashboard::PutNumber("drive axis 5", driveStick.GetRawAxis(Constants::tankDriveRightAxis));
		SmartDashboard::PutNumber("gyro", gyro->GetYaw());

		if (driveStick.GetRawButton(Constants::liftUpButton))
//			lift.Lift(driveStick.GetRawAxis(Constants::liftUpAxis)/2.0 + 0.5);
			liftVal = driveStick.GetRawAxis(Constants::liftUpAxis)/2.0 + 0.5;
		else if (driveStick.GetRawButton(Constants::liftDownButton))
//			lift.Lift(-1*(driveStick.GetRawAxis(Constants::liftDownAxis)/2.0 + 0.5));
			liftVal = -1 * driveStick.GetRawAxis(Constants::liftUpAxis)/2.0 + 0.5;


		lift->Set(ControlMode::PercentOutput, liftVal);
	}

}

void Robot::Autonomous()
{

	robotDrive.TankDrive(0.5,0.5);

	frc::Wait(0.3);

	robotDrive.TankDrive(0.0,0.0);

}

START_ROBOT_CLASS(Robot)
