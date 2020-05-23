#pragma once
#include <vector>
#include "Expression.h"

//����� ������� ���������� ���������
class EquationSystem
{
	//������ ���������
	std::vector<Expression> System;
	//�������
	size_t N;
	std::vector<double> Approx;

public:

	//�������� �������
	size_t Size() const { return System.size(); }
	//������� ������� �� ����� � ������ Filename
	explicit EquationSystem(const std::string& Filename);
	//������ ������� � �������� ������������
	std::vector<double> Solve();

};

