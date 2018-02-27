#include "ctre/Phoenix.h"
#include "WPILib.h"
#include "Pneumatics.h"
#include "Constants.h"
#include <math.h>

#ifndef SRC_GRIPPER_H_
#define SRC_GRIPPER_H_

class Gripper {
public:
	Gripper(int,int,int,int,int,int);
	void setHolder(bool open);
	void setDropper(bool open);
	void setMotors(float power);
	bool getHolder();
	bool getDropper();
private:
	VictorSPX lt, rt;
	Pneumatics holder, dropper;
};

#endif
