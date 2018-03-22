/*
 * DriveTrain.h
 *
 *  Created on: Jan 13, 2018
 *      Author: Joseph Mattson
 */
#include <networktables/NetworkTable.h>
#include <math.h>
#include <vector>

#ifndef SRC_AIMER_H_
#define SRC_AIMER_H_


class Aimer
{

public:
	Aimer(float DistanceBetweenCameras);
	double getAverageAngle();
	double getDistance();
private:
	std::shared_ptr<NetworkTable> table;
	float distanceBetweenCameras;
};


#endif /* SRC_AIMER_H_ */
