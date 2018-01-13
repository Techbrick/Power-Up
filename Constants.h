#ifndef _CONSTANTS_H
#define _CONSTANTS_H

namespace Constants
{

	static constexpr int frontLeftDriveChannel = 12;
	static constexpr int rearLeftDriveChannel = 13;
	static constexpr int frontRightDriveChannel = 14;
	static constexpr int rearRightDriveChannel = 15;
	static constexpr int driveStickChannel = 0;
	static constexpr int operatorStickChannel = 99;

	static constexpr bool frontLeftInverted = false;
	static constexpr bool rearLeftInverted = false;
	static constexpr bool frontRightInverted = true;
	static constexpr bool rearRightInverted = false;

	static constexpr int tankDriveLeftAxis = 1;
	static constexpr int tankDriveRightAxis = 5;

	static constexpr bool liftChannel = 99;
	static constexpr int liftUpButton = 7;
	static constexpr int liftUpAxis = 4;
	static constexpr int liftDownButton = 6;
	static constexpr int liftDownAxis = 3;

}

#endif
