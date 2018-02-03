#include "WPILib.h"
#include "Constants.h"
#include <math.h>
#include <fstream>

#ifndef SRC_PIDLOOP_H
#define SRC_PIDLOOP_H

class PIDLoop {
private:
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
  float minTurnSpeed;
  float maxTurnSpeed;

  float lastLeftUltrasonic = 0;
  float lastRightUltrasonic = 0;

public:
	PIDLoop();
	void resetPIDAngle();
	void setAngle(float pAngleInput, float iAngleInput, float dAngleInput);
	void setMinTurnSpeed(float minTurnSpeed_);
	void setMaxTurnSpeed(float maxTurnSpeed_);
	float PIDAngle(float yaw, float desiredAngle);
	float getAngleP() { return k_p_Angle; }
	float getAngleI() { return k_i_Angle; }
	float getAngleD() { return k_d_Angle; }

};

#endif
