#include "WPILib.h"
#include "math.h"
#include <vector>
#include <memory>
#include "Constants.h"

#ifndef SRC_AIMER_H
#define SRC_AIMER_H

#define PI 3.14159265

class Aimer
{
public:
	Aimer();

	std::shared_ptr<NetworkTable> table;

	float GetHorizontalAngles();
};

#endif
