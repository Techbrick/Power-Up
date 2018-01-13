/*
 * SuperLifter.h
 *
 *  Created on: Jan 13, 2018
 *      Author: Joseph Mattson
 */

#include "ctre/Phoenix.h"
#include "Constants.h"

#ifndef SRC_SUPERLIFTER_H_
#define SRC_SUPERLIFTER_H_


class SuperLifter
{

public:
	SuperLifter(int l);
	void Lift(float pow);

private:
	TalonSRX lift;

};


#endif /* SRC_SUPERLIFTER_H_ */
