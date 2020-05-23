#pragma once
#include <cmath>

namespace Math
{
	//��������� ����� ��� ���������
	constexpr double SMALL_NUMBER = 1.0e-8;
	constexpr double SMALLER_NUMBER = 5.0e-2;

	//������� ��� ��������: ��������� �� �������� Val � ���������� [Left:Right]
	template <class T>
	bool InRange(const T& Val, const T& Left, const T& Right)
	{
		if (Left > Right)
		{
			return Val >= Right && Val <= Left;
		}

		return Val >= Left && Val <= Right;
	}

	//������� "������" �������� � �������� [Min; Max]
	template <class T>
	T Clamp(const T& Min, const T& Max, const T& Val)
	{
		if (InRange(Val, Min, Max))
			return Val;
		if (abs(Max - Val) > abs(Min - Val))
			return Min;

		return Max;
	}

	//������� "�������������� �����" � ��������� Tolerance
	bool NearlyEqual(const double X, const double Y, const double Tolerance = SMALL_NUMBER)
	{
		return abs(X - Y) <= Tolerance;
	}

	//������� � ������� (���������������)
	template <class Iter>
	double Median(Iter begin, Iter end)
	{
		const size_t size = (end - begin) + 1;
		size_t idx = (size) / 2;

		if (size == 1)
			return *begin;

		if (size == 2)
			return (*begin + *(end - 1)) / 2;

		if (size % 2 == 0)
			return (*(begin + idx) + *(begin + idx - 1)) / 2;

		return *(begin + idx);
	}
}