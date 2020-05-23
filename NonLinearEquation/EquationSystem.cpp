#include "EquationSystem.h"
#include <fstream>
#include <iostream>
#include <string>
#include "Matrix.h"

//��������� �������� �� �����
bool sign(double x)
{
	if (x < 0)
		return true;
	else return false;
}

//���������, ���� �� � ������� "���������" �������� (inf, nan(ind))
bool IsCorrect(const std::vector<double>& Vec)
{
	const auto It = std::find_if(Vec.cbegin(), Vec.cend(), [&](double x)
	{
		return isnan(x);
	});

	return It == Vec.cend();
}

//��������� ������������� ������ �������
bool check(const std::vector<double>& Prev, const std::vector<double>& Cur)
{
	for (size_t i = 0; i < Prev.size(); ++i)
	{
		if (abs(Prev[i] - Cur[i]) >= 1.0e-8)
			return false;
	}

	return true;
}

//�������� ������� �� ������
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

//������� �� ������� ������
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

//�������� ������� �� �����
EquationSystem::EquationSystem(const std::string & Filename)
{
	std::fstream file(Filename);

	if (file)
	{
		//���� �������
		file >> N;
		Approx.resize(N);
		for (size_t i = 0; i < N; ++i)
			file >> Approx[i];

		file.ignore();
	}

	while (file)
	{
		//���� ������� ��������� � ������� � �������������� ���������
		std::string Expr;
		std::getline(file, Expr);

		if (Expr.empty())
			break;

		System.push_back(ExpressionBuilder::BuildExpression(Expr));
	}

	//����� ��������� � ������ ������� ����������
	auto It = std::find_if(System.begin(), System.end(), [&](Expression& e) {
		return e.GetVariablesCount() == N;
	});

	//� ��������� ����������
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
		//���� ��� ��������� ��������, ������� �� ������
		throw std::runtime_error("Bad equation system");
	}
}

//������� ������� ���������������� ������� �������
std::vector<double> EquationSystem::Solve()
{
	//���������� ��������
	int Iter = 1000;
	//������� �����
	Matrix J(N, Size());
	//������-�������
	std::vector<double> F(N);

	//������ ������� �����
	for (size_t i = 0; i < J.GetRows(); ++i)
	{
		for (size_t j = 0; j < J.GetColumns(); ++j)
		{
			J[i][j] = System[i].GetPartialDerivation(Approx, j+1);
		}
	}
	
	//�������� �
	J = J.Inverse();

	//������� �������
	while (Iter-- > 0)
	{
		//������� �������� ������� � �����
		for (size_t i = 0; i < N; ++i)
		{
			F[i] = System[i].GetValue(Approx);
		}

		//��������
		auto t = Multiply(J, F);
		//��������, �������� ��������� �����������
		auto NextApprox = Subtract(Approx, t);

		//���� ��� ���������� �����, ���������� ����������
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


