/*
 * DriveTrain.h
 *
 *  Created on: Jan 13, 2018
 *      Author: Joseph Mattson
 */

#include "ctre/Phoenix.h"
#include "Constants.h"

#ifndef SRC_DRIVETRAIN_H_
#define SRC_DRIVETRAIN_H_


class DriveTrain
{

public:
	DriveTrain(int lf, int rf, int lr, int rrr);
	void TankDrive(float left, float right);
	void ArcadeDrive(float fwd, float turn);

private:
	TalonSRX fl,fr,rl,rr;

};


#endif /* SRC_DRIVETRAIN_H_ */
