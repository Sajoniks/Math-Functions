#include "EquationSystem.h"
#include <fstream>
#include <iostream>
#include <string>
#include "Matrix.h"

//Проверить знаковое ли число
bool sign(double x)
{
	if (x < 0)
		return true;
	else return false;
}

//Проверить, есть ли в векторе "ошибочные" значения (inf, nan(ind))
bool IsCorrect(const std::vector<double>& Vec)
{
	const auto It = std::find_if(Vec.cbegin(), Vec.cend(), [&](double x)
	{
		return isnan(x);
	});

	return It == Vec.cend();
}

//Проверить необходимость нового расчёта
bool check(const std::vector<double>& Prev, const std::vector<double>& Cur)
{
	for (size_t i = 0; i < Prev.size(); ++i)
	{
		if (abs(Prev[i] - Cur[i]) >= 1.0e-8)
			return false;
	}

	return true;
}

//Умножить матрицу на вектор
std::vector<double> Multiply(Matrix& Left, std::vector<double>& Right)
{
	if (Left.GetColumns() == Right.size())
	{
		std::vector<double> r(Right.size());

		for (size_t k = 0; k < Left.GetColumns(); ++k)
		{
			for (size_t j = 0; j < Left.GetRows(); ++j)
			{
				r[k] += Left[k][j] * Right[j];
			}
		}

		return r;
	}
	else
	{
		return std::vector<double>();
	}
}

//Вычесть из вектора вектор
std::vector<double> Subtract(std::vector<double>& Left, std::vector<double>& Right)
{
	if (Left.size() == Right.size() && !Left.empty())
	{
		std::vector<double> r(Left.size());

		for (size_t i = 0; i < Left.size(); ++i)
		{
			r[i] = Left[i] - Right[i];
		}

		return r;
	}
	else
	{
		return std::vector<double>();
	}
}

//Создание системы из файла
EquationSystem::EquationSystem(const std::string & Filename)
{
	std::fstream file(Filename);

	if (file)
	{
		//Ввод порядка
		file >> N;
		Approx.resize(N);
		for (size_t i = 0; i < N; ++i)
			file >> Approx[i];

		file.ignore();
	}

	while (file)
	{
		//Ввод каждого выражения и перевод в математическое выражение
		std::string Expr;
		std::getline(file, Expr);

		if (Expr.empty())
			break;

		System.push_back(ExpressionBuilder::BuildExpression(Expr));
	}

	//Найти выражение с полным набором переменных
	auto It = std::find_if(System.begin(), System.end(), [&](Expression& e) {
		return e.GetVariablesCount() == N;
	});

	//И дополнить оставшиеся
	if (It != System.end())
	{
		auto Iter = System.begin();
		for (size_t i = 0; i < System.size(); ++i)
		{
			if (Iter != It && Iter->GetVariablesCount() != N)
			{
				for (size_t i = 0; i < It->GetVariablesCount(); ++i)
					Iter->ChangeVariable(It->GetVariable(i), 0);
			}
			++Iter;
		}
	}
	else
	{
		//Если все выражения неполные, говорим об ошибке
		throw std::runtime_error("Bad equation system");
	}
}

//Решение системы модифицированным методом Ньютона
std::vector<double> EquationSystem::Solve()
{
	//Количество итераций
	int Iter = 1000;
	//Матрица Якоби
	Matrix J(N, Size());
	//Вектор-функция
	std::vector<double> F(N);

	//Строим матрицу Якоби
	for (size_t i = 0; i < J.GetRows(); ++i)
	{
		for (size_t j = 0; j < J.GetColumns(); ++j)
		{
			J[i][j] = System[i].GetPartialDerivation(Approx, j+1);
		}
	}
	
	//Обращаем её
	J = J.Inverse();

	//Находим решение
	while (Iter-- > 0)
	{
		//Считаем значения функций в точке
		for (size_t i = 0; i < N; ++i)
		{
			F[i] = System[i].GetValue(Approx);
		}

		//Умножаем
		auto t = Multiply(J, F);
		//Вычитаем, получаем следующее приближение
		auto NextApprox = Subtract(Approx, t);

		//Если оно достаточно точно, прекращаем вычисления
		if (check(Approx, NextApprox))
		{
			Approx = NextApprox;
			break;
		}
		else
			Approx = NextApprox;
	}

	if (IsCorrect(Approx))
		return Approx;
	else
		throw std::runtime_error("No solution found for given approximation");
}


