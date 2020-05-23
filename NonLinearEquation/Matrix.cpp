#include "Matrix.h"
#include <istream>

#include <iostream>


Matrix::Matrix(std::initializer_list<std::initializer_list<double>> Init):
	M(Init.begin()->size()), N(Init.size())
{
	Ptr = new double*[M];

	for (size_t i = 0; i < M; ++i)
	{
		Ptr[i] = new double[N];
		
		for (size_t j = 0; j < N; ++j)
			Ptr[i][j] = *((Init.begin() + i)->begin() + j);
	}
}

Matrix::Matrix(std::vector<double>& vec):
	M(vec.size()), N(1)
{
	Ptr = new double*[N];
	Ptr[0] = new double[M];

	std::copy(vec.begin(), vec.end(), Ptr[0]);
}

Matrix::Matrix(const Matrix & Source) : Matrix(Source.M, Source.N)
{
	for (size_t i = 0; i < M; ++i)
	{
		std::copy(Source.Ptr[i], Source.Ptr[i] + N, Ptr[i]);
	}
}

Matrix Matrix::operator*(Matrix & Right) const
{
	if (N == Right.M)
	{
		Matrix Result(M, Right.N);

		for (size_t i = 0; i < Result.M; ++i)
		{
			for (size_t j = 0; j < Result.N; ++j)
			{
				for (size_t k = 0; k < M; ++k)
					Result[i][j] += Ptr[i][k] * Right[k][j];
			}
		}

		return Result;
	}
	else
	{
		throw std::exception("Size of matrices doesn't match for multiplication");
		return *this;
	}
}

//Обращение матрицы методом LU-разложения
Matrix Matrix::Inverse() const
{
	const int N = GetColumns();
	Matrix lu(N);
	Matrix inv(N);

	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			double s = 0;
			for (int k = 0; k < (i <= j ? i : j); ++k)
				s += lu[i][k] * lu[k][j];

			if (i <= j)
				lu[i][j] = Ptr[i][j] - s;
			else if (abs(lu[j][j]) > 1.0e-20)
				lu[i][j] = (Ptr[i][j] - s) / lu[j][j];
			else
				throw std::runtime_error("No inverse");
		}
	}

	for (int j = N - 1; j >= 0; --j)
	{
		for (int i = N-1; i >= 0; --i)
		{
			double s = 0;
			if (i < j)
			{
				for (int k = i + 1; k < N; ++k)
					s += lu[i][k] * inv[k][j];

				inv[i][j] = -(s / lu[i][i]);
			}
			else if (i > j)
			{
				for (int k = j + 1; k < N; ++k)
					s += inv[i][k] * lu[k][j];

				inv[i][j] = -s;
			}
			else
			{
				for (int k = j + 1; k < N; ++k)
					s += lu[j][k] * inv[k][j];

				inv[j][j] = (1 - s) / lu[j][j];
			}
		}
	}

	return inv;
}

void Matrix::operator=(const Matrix & Source)
{
	for (size_t i = 0; i < M; ++i)
	{
		std::copy(Source.Ptr[i], Source.Ptr[i] + N, Ptr[i]);
	}
}

Matrix Matrix::Identity(size_t n)
{
	Matrix I(n);

	for (size_t i = 0; i < n; ++i)
		I[i][i] = 1;

	return I;
}

std::istream & operator>>(std::istream & is, Matrix & m)
{
	for (size_t i = 0; i < m.M*m.N; ++i)
	{
		is >> m.Ptr[i / m.M][i % m.N];
	}

	return is;
}
