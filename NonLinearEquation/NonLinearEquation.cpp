#include <iostream>
#include <fstream>
#include "EquationSystem.h"

int main()
{
	for (size_t i = 1; i <= 3; ++i)
	{
		std::ofstream file("out" + std::to_string(i) + ".txt");
		if (file)
		{
			try
			{
				EquationSystem s("in" + std::to_string(i) + ".txt");
				auto sol = s.Solve();

				for (size_t i = 0; i < sol.size(); ++i)
				{
					if (sol.size() / 2 == i)
						file << "X =";
					file << "\t" << (abs(sol[i]) < 0.00001 ? 0 : sol[i]) << '\n';
				}
			}
			catch (std::exception& e)
			{
				std::cerr << e.what();
				file << e.what();
			}
		}
	}
}
