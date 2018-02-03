/*
 * SuperClimber.cpp
 *
 *  Created on: Feb 3, 2018
 *      Author: Joseph Mattson
 */

#include "SuperClimber.h"

SuperClimber::SuperClimber(int climbChannel):
	climber(climbChannel)
{
	climber.ClearStickyFaults(0);
	climber.SetNeutralMode(NeutralMode::Brake);
}

void SuperClimber::Climb(float pow)
{
	climber.Set(ControlMode::PercentOutput, pow);
}
