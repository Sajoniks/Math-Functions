#include "Matrix.h"
#include <istream>

Matrix::Matrix(const Matrix & Source) : Matrix(Source.M, Source.N)
{
	for (size_t i = 0; i < M; ++i)
	{
		std::copy(Source.Ptr[i], Source.Ptr[i] + N, Ptr[i]);
	}
}

Matrix Matrix::Multiply(Matrix & Right) const
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

Matrix Matrix::Inverse() const
{
	Matrix inverse(*this);
	return inverse;
}

Matrix Matrix::operator=(const Matrix & Source)
{
	return Matrix(Source);
}

std::istream & operator>>(std::istream & is, Matrix & m)
{
	for (size_t i = 0; i < m.M*m.N; ++i)
	{
		is >> m.Ptr[i / m.M][i % m.N];
	}

	return is;
}
