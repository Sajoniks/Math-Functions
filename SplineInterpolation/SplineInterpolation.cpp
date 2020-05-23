#include "Spline.h"
#include <iostream>

int main()
{
	try
	{
		Function f1("in1.txt");
		CubicSpline s1(f1);
		s1.SerializeSplineData("out1.txt");

		Function f2("in2.txt");
		CubicSpline s2(f2);
		s2.SerializeSplineData("out2.txt");

		Function f3("in3.txt");
		CubicSpline s3(f3);
		s3.SerializeSplineData("out3.txt");
	}
	catch (std::exception& e)
	{
		std::cerr << e.what();
	}
}