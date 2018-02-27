/*
 * Positionator.h
 *
 *  Created on: Feb 17, 2018
 *      Author: Joseph Mattson
 */

#include "WPILib.h"

#ifndef SRC_POSITIONATOR_H_
#define SRC_POSITIONATOR_H_

struct Position
{
	float xPosition;
	float yPosition;
};

class Positionator {
public:
	Positionator();
	Position findPosition(float angle, float leftEncoder, float rightEncoder);
private:
	float oldLeftEncoder;
	float oldRightEncoder;
	float oldx;
	float oldy;
};

#endif
