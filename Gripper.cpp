#include "Gripper.h"

Gripper::Gripper()
{
	lt.SetInverted(Constants::gLeftInverted);
	rt.SetInverted(Constants::gRightInverted);

	lt.ClearStickyFaults(0);
	rt.ClearStickyFaults(0);
	lt.SetNeutralMode(NeutralMode::Brake);
	rt.SetNeutralMode(NeutralMode::Brake);

	lt.ConfigSelectedFeedbackSensor(motorcontrol::FeedbackDevice::QuadEncoder, 0, 0);
	rt.ConfigSelectedFeedbackSensor(motorcontrol::FeedbackDevice::QuadEncoder, 0, 0);
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

void Gripper::setPneumatics(bool open)
{
	lp.set(open);
	rp.set(open);
}



