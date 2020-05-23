#pragma once
#include <memory>
#include <sstream>
#include <vector>

//����� �������
class Matrix
{
	friend std::istream& operator>>(std::istream& is, Matrix& m);

	//���-�� �����
	const size_t N;
	//���-�� ��������
	const size_t M;
	
	//�������
	double** Ptr;

	//������ ������������� ��������� �������� �������
	bool bCachedInverse;

public:

	//����������� �� ������� ������������� (��� �������� �������� ������� �������)
	explicit Matrix(std::initializer_list<std::initializer_list<double>> Init);
	//����������� ������� � ������� (������-�������)
	Matrix(std::vector<double>& vec);

	//������� N x N
	explicit Matrix(size_t n) :
		Matrix(n, n) {}

	//������� M x N
	explicit Matrix(size_t m, size_t n) :
		N(n), M(m), bCachedInverse(false)
	{
		Ptr = new double*[M];
			
		for (size_t i = 0; i < M; ++i)
		{
			Ptr[i] = new double[N];
			std::fill(Ptr[i], Ptr[i] + N, 0);
		}
	}
	
	//����������� �������
	Matrix(const Matrix& Source);

	//������ �������
	Matrix() : N(0), M(0) {}

	//��������� ������
	Matrix operator*(Matrix& Right) const;

	//��������� �������
	Matrix Inverse() const;
	
	//���-�� �����
	size_t GetRows() const { return N; }
	//���-�� ��������
	size_t GetColumns() const { return M; }

	//�������� i-������
	inline double*& operator[] (size_t i);

	//����������� �������
	void operator=(const Matrix& Source);

	//���������� �������� �������
	~Matrix()
	{
		for (size_t i = 0; i < M; ++i)
		{
			delete[] Ptr[i];
		}

		delete[] Ptr;
	}

	//������� ������� � ������
	operator std::string() const
	{
		std::stringstream ss;

		for (size_t i = 0; i < M; ++i)
		{
			for (size_t j = 0; j < N; ++j)
			{
				ss << Ptr[i][j] << '\t';
			}

			ss << std::endl;
		}

		return ss.str();
	}

	//������� ��������� �������
	static Matrix Identity(size_t n);
};

inline double*& Matrix::operator[](size_t i)
{
	return Ptr[i];
}