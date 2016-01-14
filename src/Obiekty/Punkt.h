/*
 * Punkt.h
 *
 *  Created on: 04-03-2014
 *      Author: przemo
 */

#ifndef PUNKT_H_
#define PUNKT_H_

class Punkt
{
public:
	double x;
	double y;

	Punkt()
	{
		x=0;
		y=0;
	}
	Punkt(double nX,double nY)
	{
		x=nX;
		y=nY;
	}
	Punkt(const Punkt& p)
	{
		x=p.x;
		y=p.y;
	}
};

typedef Punkt Wektor;

#endif /* PUNKT_H_ */