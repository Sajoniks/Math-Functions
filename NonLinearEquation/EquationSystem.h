#pragma once
#include <vector>
#include "Expression.h"

//Класс системы нелинейных уравнений
class EquationSystem
{
	//Вектор выражений
	std::vector<Expression> System;
	//Порядок
	size_t N;
	std::vector<double> Approx;

public:

	//Получить порядок
	size_t Size() const { return System.size(); }
	//Создать систему из файла с именем Filename
	explicit EquationSystem(const std::string& Filename);
	//Решить систему с заданным приближением
	std::vector<double> Solve();

};

