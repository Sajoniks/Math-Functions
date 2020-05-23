#include "DiagonalMatrix.h"
#include <iostream>


typedef std::vector<double> Vector;

//Расчёт выполняется по алгоритму из учебника Вержбицкого
Vector DiagonalMatrix::GetPlot() const
{
	Vector delta(n, 0);
	Vector lambda(n, 0);
	Vector plot(n, 0);

	//Прямой ход
	for (int i = 0; i < (int)n; ++i)
	{
		double div = c[i] + b[i] * delta[(i - 1 < 0 ? 0 : i-1)];

		//В случае деления на ноль выбрасывается исключение, чтобы оповестить пользователя
		if (abs(div) >= DBL_EPSILON)
		{
			delta[i] = -(d[i] / div);
			lambda[i] = (r[i] - b[i] * lambda[(i - 1 < 0 ? 0 : i - 1)]) / div;
		}
		else
		{
			throw std::runtime_error("Zero divide while plotting");
		}
	}

	//Обратный ход
	for (int i = n - 1; i >= 0; --i)
	{
		plot[i] = delta[i] * plot[(i + 1 >= (int)n ? 0 : i + 1)] + lambda[i];
	}

	return plot;
}

double& DiagonalMatrix::operator()(char line, size_t idx)
{
	switch (line)
	{
	case BOTTOM_DIAGONAL:
		return b[idx];
	case MAIN_DIAGONAL:
		return c[idx];
	case TOP_DIAGONAL:
		return d[idx];
	case SOLUTION:
		return r[idx];
	}
}

std::istream& operator>>(std::istream& is, DiagonalMatrix& m)
{
	int align = -1;
	for (size_t i = 0; i < m.n; ++i)
	{
		int read = m.n;

		if (align < 0)
			m.b[i] = 0;
		else
		{
			read -= align;
			while (align > 0)
			{
				int buf; is >> buf;
				--align;
			}

			align = m.n - read;
			is >> m.b[i];
			--read;
		}

		is >> m.c[i];
		--read;

		if (align == (int)m.n - 2)
			m.d[i] = 0;
		else
		{
			is >> m.d[i];
			--read;
		}

		while (read-- > 0)
		{
			int buf; is >> buf;
		}

		is >> m.r[i];
		++align;
	}
	return is;
}