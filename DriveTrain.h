/*
 * DriveTrain.h
 *
 *  Created on: Jan 13, 2018
 *      Author: Joseph Mattson
 */

#include "ctre/Phoenix.h"
#include "Constants.h"
#include "PIDLoop.h"
#include "AHRS.h"
#include <math.h>

#ifndef SRC_DRIVETRAIN_H_
#define SRC_DRIVETRAIN_H_


class DriveTrain
{

public:
	DriveTrain(int lf, int rf, int lr, int rrr, AHRS& g, PIDLoop p);
	void TankDrive(float left, float right);
	void ArcadeDrive(float fwd, float turn);
	float GetEncoders();
	float GetLeftEncoder() {return fl.GetSelectedSensorPosition(0);}
	float GetRightEncoder() {return fr.GetSelectedSensorPosition(0);}
	void DriveStraight(float left, float right);
	void DriveStraightArcade(float fwd);
	void DriveStraightReset();
	void DriveStraightDistance(float fwd, float dist);
	void FieldOrientedDrive(float up, float right);
	float AngleConvert(float angle);

private:
	TalonSRX fl,fr,rl,rr;
	AHRS& gyro;
	PIDLoop pid;
	float lastGyro;
	bool straightening;

};


#endif /* SRC_DRIVETRAIN_H_ */
