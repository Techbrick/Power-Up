/*
 * DriveTrain.cpp
 *
 *  Created on: Jan 13, 2018
 *      Author: Joseph Mattson
 */

#include "DriveTrain.h"

DriveTrain::DriveTrain(int lf, int rf, int lr, int rrr, AHRS& g, PIDLoop p):
	fl(lf),
	fr(rf),
	rl(lr),
	rr(rrr),
	gyro(g),
	pid(p),
	lastGyro(0),
	straightening(false)
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

	rl.Follow(fl);
	rr.Follow(fr);

	fl.ConfigSelectedFeedbackSensor(motorcontrol::FeedbackDevice::QuadEncoder, 0, 0);
	fr.ConfigSelectedFeedbackSensor(motorcontrol::FeedbackDevice::QuadEncoder, 0, 0);

	fl.SetSensorPhase(Constants::frontLeftEncoderInverted);
	fr.SetSensorPhase(Constants::frontRightEncoderInverted);

	fl.SetSelectedSensorPosition(0,0,0);
	fr.SetSelectedSensorPosition(0,0,0);
}

void DriveTrain::TankDrive(float left, float right)
{
	if (fabs(left) < .05)
	{
		fl.Set(ControlMode::PercentOutput, 0.0);
	}
	if (fabs(right) < .05)
	{
		fr.Set(ControlMode::PercentOutput, 0.0);
	}
	if (left > 1)
	{
		right = right/left;
		left = 1;
	}
	if (right > 1)
	{
		left = left/right;
		right = 1;
	}

	fl.Set(ControlMode::PercentOutput, left);
	fr.Set(ControlMode::PercentOutput, right);
}


void DriveTrain::ArcadeDrive(float fwd, float turn)
{
	float left = fwd + turn;
	float right = fwd - turn;
	if (fabs(left) > fabs(right) && fabs(left) > 1)
	{
		right /= fabs(left);
		left = copysign(1.0,left);
	}
	else if (fabs(right) > fabs(left) && fabs(right) > 1)
	{
		left /= fabs(right);
		right = copysign(1.0,right);
	}

	SmartDashboard::PutNumber("Arcade Forward" , fwd);
	SmartDashboard::PutNumber("Arcade Turn" , turn);
	SmartDashboard::PutNumber("Arcade Left" , left);
	SmartDashboard::PutNumber("Arcade Right" , right);

	fl.Set(ControlMode::PercentOutput,left);
	fr.Set(ControlMode::PercentOutput,right);
}

void DriveTrain::DriveStraight(float left, float right)
{
	float oldP = pid.getAngleP();
	pid.setAngle(Constants::driveStraightPValue * (fabs(left) + fabs(right)) * 0.5, pid.getAngleI(), pid.getAngleD());
	if (!straightening)
	{
		lastGyro = this->AngleConvert(gyro.GetYaw());
		straightening = true;
	}

	float error = pid.PIDAngle(this->AngleConvert(gyro.GetYaw()), lastGyro);
	left = (left - -1 * error);
	right = (right + -1 * error);

	if (fabs(left) > fabs(right) && fabs(left) > 1)
	{
		right /= fabs(left);
		left = copysign(1.0,left);
	}
	else if (fabs(right) > fabs(left) && fabs(right) > 1)
	{
		left /= fabs(right);
		right = copysign(1.0,right);
	}

	SmartDashboard::PutNumber("Drive Straight left", left);
	SmartDashboard::PutNumber("Drive Straight right", right);
	SmartDashboard::PutNumber("Drive Straight error", error);

	pid.setAngle(oldP, pid.getAngleI(), pid.getAngleD());
	this->TankDrive(left,right);
}

void DriveTrain::DriveStraightArcade(float fwd)
{
	this->DriveStraight(fwd,fwd);
}

void DriveTrain::DriveStraightReset()
{
	lastGyro = 0;
	straightening = false;
	pid.resetPIDAngle();
}

float DriveTrain::GetEncoders()
{
	return (fl.GetSelectedSensorPosition(0) + fr.GetSelectedSensorPosition(0))/2.0;
}

void DriveTrain::DriveStraightDistance(float fwd, float dist)
{
	float fles = fl.GetSelectedSensorPosition(0);
	float fres = fl.GetSelectedSensorPosition(0);

	SmartDashboard::PutNumber("Left Encoder", fles);
	SmartDashboard::PutNumber("Right Encoder", fres);
	SmartDashboard::PutNumber("Left Encoder Inches", fles / Constants::ticksPerInch);
	SmartDashboard::PutNumber("Right Encoder Inches", fres / Constants::ticksPerInch);

	int failsafe = 0;

	SmartDashboard::PutNumber("Left distance to thing", fabs(fl.GetSelectedSensorPosition(0) - fles) * Constants::encoderFactor);
	SmartDashboard::PutNumber("Right distance to thing", fabs(fr.GetSelectedSensorPosition(0) - fres) * Constants::encoderFactor);

	float distError = (dist * Constants::ticksPerInch - fabs((fl.GetSelectedSensorPosition(0) - fles) * Constants::encoderFactor) + dist * Constants::ticksPerInch - fabs((fr.GetSelectedSensorPosition(0) - fres) * Constants::encoderFactor)) / 2.0;
	float rampDistance = fabs(Constants::startRampDownFactor * fwd);
	float power = fwd;

	while(distError > 1 && failsafe < 400)
	{

		if (distError / Constants::ticksPerInch < rampDistance)
			power = fwd * ((distError / Constants::ticksPerInch) / rampDistance);

		if (fabs(power) < Constants::minForwardPower)
			power = copysign(Constants::minForwardPower,power);

		this->DriveStraight(power, power);
		failsafe++;
		frc::Wait(0.005);

		distError = (dist * Constants::ticksPerInch - fabs((fl.GetSelectedSensorPosition(0) - fles) * Constants::encoderFactor) + dist * Constants::ticksPerInch - fabs((fr.GetSelectedSensorPosition(0) - fres) * Constants::encoderFactor)) / 2.0;

		SmartDashboard::PutBoolean("Driving Straight to Distance", true);
		SmartDashboard::PutNumber("Driving Straight Distance Error", distError);
	}

	SmartDashboard::PutBoolean("Driving Straight to Distance", false);
}

void DriveTrain::FieldOrientedDrive(float up, float right)
{
	float fwd = sqrt(pow(up,2)+pow(right,2));
	if (fwd > 1) fwd = 1;
	if (fwd < -1) fwd = -1;
	float angle = acos(up/fwd) * 57.2957796;
	angle = this->AngleConvert(angle);

	float turn = pid.PIDAngle(this->AngleConvert(gyro.GetYaw()) ,angle);
	fwd *= 1 - fabs(turn);

	this->ArcadeDrive(fwd,turn);
}

float DriveTrain::AngleConvert(float angle)
{
	return angle < 0 ? 360 + angle : angle;
}
