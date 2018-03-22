#include "Aimer.h"


Aimer::Aimer(float DistanceBetweenCameras)
{
	table = NetworkTable::GetTable("datatable");
	Aimer::distanceBetweenCameras = DistanceBetweenCameras;
}

double Aimer::getAverageAngle()
{
	std::vector<double> rightAngles = Aimer::table->GetNumberArray("Horizontal-Angles-1", {404});
	std::vector<double> leftAngles = Aimer::table->GetNumberArray("Horizontal-Angles-0", {404});

	if(rightAngles[0] == 404 || leftAngles[0] == 404)
		return 404;




	return (abs(rightAngles[0]) + abs(leftAngles[0])) / 2;
}

double Aimer::getDistance()
{
	std::vector<double> rightAngles = Aimer::table->GetNumberArray("Horizontal-Angles-1", {404});
	std::vector<double> leftAngles = Aimer::table->GetNumberArray("Horizontal-Angles-0", {404});

	if(rightAngles[0] == 404 || leftAngles[0] == 404)
		return 404;
	//Angles are -30 to 30, convert to 0 - 180
	double A = leftAngles[0] + 90;
	double B = rightAngles[0] + 90;
	double C = 180 - A - B;
	A *= 3.1415 / 180;
	B *= 3.1415 / 180;
	C *= 3.1415 / 180;
	double step1 = (Aimer::distanceBetweenCameras * sin(C)) / sin(A);

	return sin(B) * step1;
}
