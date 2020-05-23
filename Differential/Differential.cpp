#include <iostream>
#include <fstream>
#include <string>
#include "Expression.h"

//Сетка
struct SGrid
{
	explicit  SGrid(size_t Divisions, double Step, double StartPoint = 0) :
		Divisions(Divisions), Step(abs(Step)), StartPoint(StartPoint) {}

	//Координата Node узла
	double	operator()(size_t Node)	const;

	//Начальная точка
	double StartPoint = 0;

	//Количество разбиений
	size_t Divisions;

	double	GetStep()				const	{ return Step; };
	void	SetStep(double Step)			{ Step = abs(Step); }

private:

	//Шаг
	double Step;
};

inline double SGrid::operator()(size_t Node) const
{
	if (Node > Divisions)
		Node = Divisions;

	return StartPoint + Step * Node;
}

int main()
{
	SGrid Grid(50, 0.1);
	const double t = Grid.GetStep();
	
	const std::string FileName = "in3.txt";
	const std::string OutFileName = "out" + FileName.substr(2);
	std::ifstream File(FileName);
	std::ofstream OFile(OutFileName);
	
	std::string ExpressionString;
	std::getline(File, ExpressionString);
	Expression Expr = ExpressionBuilder::BuildExpression(ExpressionString);

	//Задача Коши
	double Yn;
	File >> Yn;

	OFile << Grid.StartPoint << '\t' << Yn << '\n';
	
	for (size_t i = 0; i < Grid.Divisions; ++i)
	{
		double Tn = Grid(i);

		double k1 = Expr({Tn, Yn});
		double k2 = Expr({ Tn + t / 3, Yn + t * k1 / 3 });
		double k3 = Expr({Tn +2*t/3, Yn + 2*k2*t/3	});

		Yn = t*(k1 + 3 * k2)/4 + Yn;

		OFile << Grid(i + 1) << '\t' << Yn << '\n';
	}
}
