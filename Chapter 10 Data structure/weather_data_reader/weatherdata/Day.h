#pragma once
#include <vector>

namespace Dates {
	const double not_a_measuring = -1000;

	// every Day contains 24 measurings filld with -1000 if there isn't a real measuring
	struct Day
	{
		bool is_empty; //curr not used
		std::vector <double> measuring;
		Day() : is_empty(true), measuring(24, not_a_measuring) {}
	};
}