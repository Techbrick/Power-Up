#ifndef _CONSTANTS_H
#define _CONSTANTS_H

namespace Constants
{

	static constexpr int frontLeftDriveChannel = 12;
	static constexpr int rearLeftDriveChannel = 13;
	static constexpr int frontRightDriveChannel = 14;
	static constexpr int rearRightDriveChannel = 15;
	static constexpr int driveStickChannel = 0;
	static constexpr int operatorStickChannel = 1;

	static constexpr bool gRightInverted = false;
	static constexpr bool gLeftInverted = false;

	static constexpr bool frontLeftInverted = true;
	static constexpr bool rearLeftInverted = false;
	static constexpr bool frontRightInverted = false;
	static constexpr bool rearRightInverted = false;

	static constexpr int tankDriveLeftAxis = 1;
	static constexpr int tankDriveRightAxis = 5;
	static constexpr int arcadeDriveForwardAxis = 1;
	static constexpr int arcadeDriveTurnAxis = 2;
	static constexpr int fieldOrientedDriveVerticalAxis = 1;
	static constexpr int fieldOrientedDriveHorizontalAxis = 0;

	static constexpr int driveStraightButton = 6;
	static constexpr int driveStraightButtonFor12Inches = 2;
	static constexpr float driveStraightPValue = 0.0025;
	static constexpr float startRampDownFactor = 72.0;

	static constexpr int climbChannel = 99;

	static constexpr int liftChannel = 10;
	static constexpr int liftUpButton = 8;
	static constexpr int liftUpAxis = 4;
	static constexpr int liftDownButton = 7;
	static constexpr int liftDownAxis = 3;

	static constexpr float minForwardPower = 0.025; //TODO: figure this out boyz
	static constexpr float angleErrorLimit = 1.0; //TODO: change this boyz

	static constexpr float percentDrivePower = 0.5;

	static constexpr float encoderFactor = 1.0;
	static constexpr bool frontLeftEncoderInverted = false;
	static constexpr bool frontRightEncoderInverted = false;

	static constexpr int toggleDriveModeButton = 11;

	static constexpr float ticksPerInch = 432.4239803807;
	static constexpr int ticksPerRev = int(4096*1.65);
	static constexpr float wheelCircumference = 0.0254*6*3.14159265;

	static constexpr float distanceBetweenWheelsMeters = 0.70485;
	static constexpr float maxVelocity = 1.5;
	static constexpr float actualMaxVelocity = 3.0;
	static constexpr float maxAcceleration = 3.0;
	static constexpr float maxJerk = 20.0;
	static constexpr float powerPerVelocity = 2.3 * 0.5 / 3.35;

}

#endif
