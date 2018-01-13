/*
 * PIDLoop.cpp
 *
 *  Created on: Jan 13, 2018
 *      Author: Joseph Mattson
 */




#include "PIDLoop.h"

PIDLoop::PIDLoop() //:
	//filter()
{
  k_p_Angle = .01;
  k_i_Angle = .001;
  k_d_Angle = .001;
  p_Angle = 0;
  i_Angle = 0;
  d_Angle = 0;
  angle_error = 0;
  angleOutput = 0;
  last_angle_error = 0;
  angleMaxError = 2;
  iteration_time = .005;
}

void PIDLoop::resetPIDAngle() { //reset angle pid values
	p_Angle = 0;
	i_Angle = 0;
	d_Angle = 0;
}

void PIDLoop::setAngle(float pAngleInput, float iAngleInput, float dAngleInput) { //set angle PID constants
	k_p_Angle = pAngleInput;
	k_i_Angle = iAngleInput;
	k_d_Angle = dAngleInput;
}

float PIDLoop::PIDAngle(float angleOffset, float desiredAngle) {
  //put in separate loop - not a while loop - keep checking and updating every runthrough of the normal loop - boolean for if this is running to stop you from manually moving the robot while the loop is running
  std::ofstream logger; logger.open("/var/loggerFile.txt", std::ofstream::out); //start logger
  logger << "Loop entered\n";

  angle_error = angleOffset - desiredAngle; //calculate error
  angle_error = fabs(angle_error) > 180 ? 180 - angle_error : angle_error; //scale error to take shortest path
  if (desiredAngle == 0 && angleOffset > 180) {
		  angle_error = angleOffset - 360;
  }

  p_Angle = k_p_Angle * angle_error; //calculate p
  i_Angle += k_i_Angle * (angle_error * iteration_time); //calculate i
  d_Angle = k_d_Angle * ((angle_error - last_angle_error) / iteration_time); //calculate d
  angleOutput = p_Angle + i_Angle + d_Angle; //calculate output
  last_angle_error = angle_error; //set last angle error for d value



  angleOutput = fabs(angleOutput) < .14 ? copysign(.14, angleOutput) : angleOutput; //if angleOutput is below min, set to min
  angleOutput = fabs(angleOutput) > .9 ? copysign(.9, angleOutput) : angleOutput; //if angleOutput is above max, set to max
  //angleOutput = angle_error < 0 ? angleOutput : -angleOutput;
  if (fabs(angle_error) < Constants::angleErrorLimit) { //if done moving
	  i_Angle = 0;
	  angleOutput = 0;
  }
  angleOutput = -angleOutput;
  logger << p_Angle << " " << angle_error << " " << angleOutput << "\n"; //output to log file
  //frc::Wait(iteration_time);
  logger.close();

  /*SmartDashboard::PutNumber("Accumulated i", i_Angle);
  SmartDashboard::PutNumber("Desired Angle", desiredAngle);
  SmartDashboard::PutNumber("angleOffset", angleOffset);
  SmartDashboard::PutNumber("angle_error", angle_error);*/

  return angleOutput;
}
