/*
 * DriveTrain.cpp
 *
 *  Created on: Jan 13, 2018
 *      Author: Joseph Mattson
 */

#include "DriveTrain.h"

DriveTrain::DriveTrain(int lf, int rf, int lr, int rrr):
	fl(lf),
	fr(rf),
	rl(lr),
	rr(rrr)
{
	fl.SetInverted(Constants::frontLeftInverted);
	rl.SetInverted(Constants::rearLeftInverted);
	fr.SetInverted(Constants::frontRightInverted);
	rr.SetInverted(Constants::rearRightInverted);

	rl.ClearStickyFaults(0);
	rr.ClearStickyFaults(0);
	fl.ClearStickyFaults(0);
	fr.ClearStickyFaults(0);
	fl.SetNeutralMode(NeutralMode::Brake);
	fr.SetNeutralMode(NeutralMode::Brake);
	rl.SetNeutralMode(NeutralMode::Brake);
	rr.SetNeutralMode(NeutralMode::Brake);
}

void DriveTrain::TankDrive(float left, float right)
{
	if (fabs(left) < .05)
	{
		fl.Set(ControlMode::PercentOutput, 0.0);
		rl.Set(ControlMode::PercentOutput, 0.0);
	}
	if (fabs(right) < .05)
	{
		fr.Set(ControlMode::PercentOutput, 0.0);
		rr.Set(ControlMode::PercentOutput, 0.0);
	}
	fl.Set(ControlMode::PercentOutput, left);
	fr.Set(ControlMode::PercentOutput, right);
	rl.Set(ControlMode::PercentOutput, left);
	rr.Set(ControlMode::PercentOutput, right);
}

void DriveTrain::ArcadeDrive(float fwd, float turn)
{

}
