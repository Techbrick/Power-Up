/*
 * SuperClimber.h
 *
 *  Created on: Feb 3, 2018
 *      Author: Joseph Mattson
 */

#include "ctre/Phoenix.h"
#include "Constants.h"

#ifndef SRC_SUPERCLIMBER_H_
#define SRC_SUPERCLIMBER_H_

class SuperClimber
{

public:
	SuperClimber(int c);
	void Climb(float pow);

private:
	TalonSRX climber;

};

#endif /* SRC_SUPERCLIMBER_H_ */
