#include "ctre/Phoenix.h"
#include "WPILib.h"
#include "Pneumatics.h"
#include "Constants.h"
#include <math.h>

#ifndef SRC_GRIPPER_H_
#define SRC_GRIPPER_H_

class Gripper {
public:
	Gripper();
	void setPneumatics(bool open);
	void setMotors(float power);
private:
	TalonSRX lt, rt;
	Pneumatics lp, rp;
};

#endif
