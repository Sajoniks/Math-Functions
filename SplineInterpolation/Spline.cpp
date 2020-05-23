#include "Spline.h"
#include "DiagonalMatrix.h"
#include <fstream>

double Function::x(size_t i) const
{
	return Points[i].first;
}

double Function::y(size_t i) const
{
	return Points[i].second;
}

double Function::Step(size_t i) const
{
	if (i > 0 && i < Points.size())
	{
		return Points[i].first - Points[i-1].first;
	}
	else
	{
		return 0.f;
	}
}

Function::Function(const std::string& Filename)
{
	std::ifstream file(Filename);
	if (file)
	{
		double x, y;
		while (file >> x && file >> y)
		{
			Points.push_back({ x, y });
		}
	}
	else
	{
		throw std::runtime_error(Filename + ": file not found");
	}
}

CubicSpline::CubicSpline(Function InterpFunc) : IInterpolate(InterpFunc)
{
	//Создаем сегменты
	for (size_t i = 0; i < Func.Size() - 1; ++i)
	{
		if (i > 0)
			Segments.back().Length = Func.Step(i);

		Segment seg({ Func.x(i), Func.x(i + 1) });
		Segments.push_back(seg);
	}

	Segments.back().Length = Func.Step(Func.Size() - 1);
	size_t n = Func.Size();

	//Создаем трёхдиагональную матрицу для того, чтобы найти коэффициенты полиномов на каждом сегменте сплайна
	//По учебнику Самарского-Гулина
	DiagonalMatrix m(n - 2);
	for (size_t i = 1; i <= n-2; ++i)
	{
		size_t k = i - 1;
		m(DiagonalMatrix::MAIN_DIAGONAL, k) = 2 * (Segments[k].Length + Segments[k + 1].Length);

		double df1 = (Func.y(i + 1) - Func.y(i)) / Segments[k + 1].Length;
		double df2 = (Func.y(i) - Func.y(i - 1)) / Segments[k].Length;

		m(DiagonalMatrix::SOLUTION, k) = 6 * (df1 - df2);

		if (i > 1)
			m(DiagonalMatrix::BOTTOM_DIAGONAL, k) = Segments[k].Length;

		if (i < n - 2)
			m(DiagonalMatrix::TOP_DIAGONAL, k) = Segments[k+1].Length;
	}

	//Вектор с-коэффициентов
	auto c = m.GetPlot();

	//По условию с0 = сN = 0
	c.insert(c.begin(), 0);
	c.push_back(0);

	//Рассчёт коэффициентов для каждого сегмента
	//По формулам 9, 10 из учебника Самарского-Гулина
	for (size_t i = 0; i < Segments.size(); ++i)
	{
		size_t k = i + 1;
		double step = Segments[i].Length;
		Segments[i].c = c[k];
		Segments[i].a = Func.y(k);
		Segments[i].d = (c[k] - c[k - 1]) / step;
		Segments[i].b = (step / 2) * c[k] - ((step*step) / 6) * Segments[i].d + (Func.y(k) - Func.y(k-1)) / step;
	}
}

double CubicSpline::operator()(double x)
{
	//Номер сегмента, куда попала точка
	size_t idx = 0;
	while (idx < Segments.size() && !Segments[idx].HasPoint(x)) ++idx;

	if (idx >= Segments.size())
		return Func.y(idx);
	else if (idx < 0)
		return Func.y(idx + 1);

	Segment& s = Segments[idx];
	double dx = (x - s.End);
	return s.a + s.b * dx + (s.c * dx*dx) / 2 + (s.d * dx*dx*dx) / 6;
}

void CubicSpline::SerializeSplineData(const std::string & Filename, size_t PointCount)
{
	std::ofstream file(Filename);
	//Рассчёт шага для того, чтобы уместить все точки
	double Step = abs(Segments.front().Begin - Segments.back().End) / PointCount;

	if (Step < 0.0001)
	{
		throw std::runtime_error("Too small step size!");
		Step = abs(Segments.front().Begin - Segments.back().End) / PointCount * 100;
	}

	for (double i = Segments.front().Begin; i <= Segments.back().End + 0.001; i += Step)
	{
		file << i << '\t' << operator()(i) << '\n';
	}
}
