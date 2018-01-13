#include "WPILib.h"
#include "Constants.h"
//#include "Filters.h"
#include <math.h>
#include <fstream>

#ifndef SRC_PIDLOOP_H
#define SRC_PIDLOOP_H

class PIDLoop {

  float k_p_Angle;
  float k_i_Angle;
  float k_d_Angle;
  float p_Angle;
  float i_Angle;
  float d_Angle;
  float angle_error;
  float last_angle_error;
  float angleOutput;
  float angleMaxError;
  float iteration_time;

  float lastLeftUltrasonic = 0;
  float lastRightUltrasonic = 0;

//  Filters filter;

public:
	PIDLoop();
	void resetPIDAngle();
	void setAngle(float pAngleInput, float iAngleInput, float dAngleInput);
	float PIDAngle(float yaw, float desiredAngle);
};

#endif
