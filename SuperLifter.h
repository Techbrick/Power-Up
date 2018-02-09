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
	SuperLifter(int l, int h);
	void Lift(float pow);
	void SetHeight(float inches);
	void Brake();
	void Reset();
	void Position();
	void SetPosition(float revs);

private:
	TalonSRX lift;
	TalonSRX help;
	float p, d, penc, tgt;

};


#endif /* SRC_SUPERLIFTER_H_ */
