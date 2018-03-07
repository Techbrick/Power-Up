//#include "Aimer.h"
//Aimer::Aimer(NetworkTable aTab, float DistanceBetweenCameras)
//{
//	Aimer::table = aTab;
//	Aimer::distanceBetweenCameras = DistanceBetweenCameras;
//}
//
//float Aimer::getAverageAngle()
//{
//	double* rightAngles = Aimer::table.GetNumberArray()("Horizontal-Angles-1", NULL);
//	double* leftAngles = Aimer::table.GetNumberArray("Horizontal-Angles-0", NULL);
//
//	if(rightAngle == NULL || leftAngles == NULL)
//		return 404;
//
//
//
//
//	return (abs(rightAngles[0]) + abs(leftAngles[0])) / 2;
//}
//
//Aimer::GetDistance()
//{
//	double* rightAngles = Aimer::table.getNumberArray("Horizontal-Angles-1", null);
//	double* leftAngles = Aimer::table.getNumberArray("Horizontal-Angles-0", null);
//
//	if(rightAngle == null || leftAngles == null)
//		return 404;
//	//Angles are -30 to 30, convert to 0 - 180
//	double A = leftAngles[0] + 90;
//	double B = rightAngles[0] + 90;
//	double C = 180 - A - B;
//	A *= pi / 180;
//	B *= pi / 180;
//	C *= pi / 180;
//	double step1 = (Aimer::distanceBetweenCameras * sin(C)) / sin(A);
//
//	return sin(B) * step1;
//}
