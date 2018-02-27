#include "Gripper.h"

Gripper::Gripper(int l, int r, int h1, int h2, int d1, int d2):
	lt(l),
	rt(r),
	holder(h1,h2),
	dropper(d1,d2)
{
	lt.SetInverted(Constants::gLeftInverted);
	rt.SetInverted(Constants::gRightInverted);

	lt.ClearStickyFaults(0);
	rt.ClearStickyFaults(0);
	lt.SetNeutralMode(NeutralMode::Brake);
	rt.SetNeutralMode(NeutralMode::Brake);
	lt.SetInverted(Constants::leftGripperInverted);
	rt.SetInverted(Constants::rightGripperInverted);
}

void Gripper::setMotors(float power)
{
	lt.Set(ControlMode::PercentOutput, power);
	rt.Set(ControlMode::PercentOutput, power);
	if (fabs(power) < .05)
	{
		lt.Set(ControlMode::PercentOutput, 0.0);
		rt.Set(ControlMode::PercentOutput, 0.0);
	}
}

void Gripper::setHolder(bool open)
{
	holder.set(open);
}

void Gripper::setDropper(bool open)
{
	dropper.set(open);
}

bool Gripper::getHolder()
{
	return holder.get();
}

bool Gripper::getDropper()
{
	return dropper.get();
}
