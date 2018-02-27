#include "Positionator.h"
#include "WPILib.h"
#include "PIDLoop.h"

Positionator::Positionator():
	 oldLeftEncoder(),
	 oldRightEncoder(),
	 oldx(),
	 oldy()
{}

Position Positionator::findPosition(float angle, float leftEncoder, float rightEncoder)
{
//		4096*1.65 ticks per revolution of the wheel
//		Wheel diameter is 6 inches
		Position p;
		float leftEncoderDifference = leftEncoder - oldLeftEncoder;
		float rightEncoderDifference = rightEncoder - oldRightEncoder;
		float average = (leftEncoderDifference+rightEncoderDifference)/2;
		float distance = average * (6*3.14159265)/(4096*1.65);
		oldx = distance * cos(angle);
		oldy = distance * sin(angle);
		oldLeftEncoder = leftEncoder;
		oldRightEncoder = rightEncoder;
		p.xPosition = oldx;
		p.yPosition = oldy;
		return p;
}
