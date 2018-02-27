/*
 * SuperLifter.cpp
 *
 *  Created on: Jan 13, 2018
 *      Author: Joseph Mattson
 */

#include "SuperLifter.h"
#include <iostream>

SuperLifter::SuperLifter(int liftChannel, int helpChannel):
	lift(liftChannel),
	help(helpChannel),
	p(0.0001),
	d(0.00001),
	penc(0),
	tgt(-1),
	zero(0)
{
	lift.ClearStickyFaults(0);
	help.ClearStickyFaults(0);

	lift.SetNeutralMode(ctre::phoenix::motorcontrol::NeutralMode::Brake);
	help.SetNeutralMode(NeutralMode::Brake);

	lift.SetInverted(Constants::liftInverted);
	help.SetInverted(Constants::helpInverted);

//	lift.ConfigSelectedFeedbackSensor(motorcontrol::FeedbackDevice::QuadEncoder, 0, 0);
//
//	lift.SetSensorPhase(Constants::liftEncoderInverted);
//
//	lift.SetSelectedSensorPosition(0,0,0);

	int absolutePosition = lift.GetSelectedSensorPosition(0) & 0xFFF; /* mask out the bottom12 bits, we don't care about the wrap arounds */
	lift.SetSelectedSensorPosition(absolutePosition, 0,0);
	lift.ConfigSelectedFeedbackSensor(motorcontrol::FeedbackDevice::CTRE_MagEncoder_Absolute, 0,0);
	lift.SetSensorPhase(Constants::liftEncoderInverted);
	lift.Config_kF(0, 0.0, 0);
	lift.Config_kP(0, 0.5, 0);
	lift.Config_kI(0, 0.0, 0);
	lift.Config_kD(0, 0.0, 0);

	help.Follow(lift);

	lift.ConfigPeakCurrentLimit(30,0);
	help.ConfigPeakCurrentLimit(30,0);
}

void SuperLifter::Lift(float pow)
{
	//if (lift.GetOutputCurrent() < Constants::lifterMaxCurrent)
//	if(true)
//	{
		lift.Set(ControlMode::PercentOutput, pow);
//		help.Set(ControlMode::PercentOutput, pow);
//		std::cout << lift.GetSelectedSensorPosition(0) << "\n";
		this->Reset();
//	}
//	else
//	{
//		this->Brake();
//	}
}

void SuperLifter::Brake()
{
	if (tgt == -1)
		tgt = lift.GetSelectedSensorPosition(0);
//	float kp = (tgt - lift.GetSelectedSensorPosition(0));
//	float kd = -1*(penc - lift.GetSelectedSensorPosition(0))/0.005;
//	float pow = kp * p + kd * d;
//	penc = lift.GetSelectedSensorPosition(0);
//	Lift(pow);
//
//	std::cout << penc << "         " << kp << "      " << kd << "         " << pow << "\n";
	lift.Set(ControlMode::Position, tgt);
//	std::cout << lift.GetSelectedSensorPosition(0) << "\n";

}

void SuperLifter::Reset()
{
	tgt = -1;
}

void SuperLifter::Position()
{
	lift.Set(ControlMode::Position, penc);
	this->Reset();
}

void SuperLifter::SetPosition(float revs)
{
	penc = revs * 4096 + lift.GetSelectedSensorPosition(0);
}

int SuperLifter::GetEncoder()
{
	return lift.GetSelectedSensorPosition(0) - zero;
}

void SuperLifter::Zero()
{
	zero = lift.GetSelectedSensorPosition(0);
}
