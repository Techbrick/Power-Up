/*
 * SuperClimber.cpp
 *
 *  Created on: Feb 3, 2018
 *      Author: Joseph Mattson
 */

#include "SuperClimber.h"

SuperClimber::SuperClimber(int c1, int c2):
	climber(c1),
	helper(c2)
{
	climber.ClearStickyFaults(0);
	climber.SetNeutralMode(NeutralMode::Brake);

	helper.ClearStickyFaults(0);
	helper.SetNeutralMode(NeutralMode::Brake);

	helper.Follow(climber);
}

void SuperClimber::Climb(float pow)
{
	climber.Set(ControlMode::PercentOutput, pow);
}
