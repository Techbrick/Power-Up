#include "Aimer.h"
#include <vector>

Aimer::Aimer()
{
	table = NetworkTable::GetTable("datatable");
}

float Aimer::GetHorizontalAngles()
{
	std::vector<double> nums = (table->GetNumberArray("Horizontal-Angles", {}));
	if (nums.size() < 1)
		return 525600.0;
	else
		return (float)nums.at(0);
}
