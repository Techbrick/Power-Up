/*
 * SuperLifter.cpp
 *
 *  Created on: Jan 13, 2018
 *      Author: Joseph Mattson
 */

#include "SuperLifter.h"

SuperLifter::SuperLifter(int liftChannel):
	lift(liftChannel)
{
//	lift.Enable();
	lift.ClearStickyFaults(0);
	lift.SetNeutralMode(NeutralMode::Brake);
}

void SuperLifter::Lift(float pow)
{
	lift.Set(ControlMode::PercentOutput, pow);
}
