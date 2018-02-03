#include <Positionator.h>
#include "WPILib.h"
#include "PIDLoop.h"
int oldLeftEncoder = 0;
int oldRightEncoder = 0;
Positionator::Positionator() {

}
struct Position
{
	float xPosition;
	float yPosition;
};
float Positionator::findPosition(float angle, int leftEncoder, int rightEncoder)
{
//		4096*1.65 ticks per revolution of the wheel
//		Wheel diameter is 6 inches
		Position p;
		int leftEncoderDifference = leftEncoder - oldLeftEncoder;
		int rightEncoderDifference = rightEncoder - oldRightEncoder;
		float average = (leftEncoderDifference+rightEncoderDifference)/2;
		float distance = average * (6*3.14159265)/(4096*1.65);
		p.xPosition = distance * cos(angle);
		p.yPosition = distance * sin(angle);
		return p;
}
