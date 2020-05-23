#pragma once
#include <memory>
#include <sstream>
#include <vector>

//класс Матрицы
class Matrix
{
	friend std::istream& operator>>(std::istream& is, Matrix& m);

	//Кол-во строк
	const size_t N;
	//Кол-во столбцов
	const size_t M;
	
	//Матрица
	double** Ptr;

	//Флажок необходимости пересчёта обратной матрицы
	bool bCachedInverse;

public:

	//Конструктор из списков инициализации (для удобства создания матрицы вручную)
	explicit Matrix(std::initializer_list<std::initializer_list<double>> Init);
	//Конвертация вектора в матрицу (вектор-столбец)
	Matrix(std::vector<double>& vec);

	//Матрица N x N
	explicit Matrix(size_t n) :
		Matrix(n, n) {}

	//Матрица M x N
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
	
	//Скопировать матрицу
	Matrix(const Matrix& Source);

	//Пустая матрица
	Matrix() : N(0), M(0) {}

	//Умножение матриц
	Matrix operator*(Matrix& Right) const;

	//Обращение матрицы
	Matrix Inverse() const;
	
	//Кол-во строк
	size_t GetRows() const { return N; }
	//Кол-во столбцов
	size_t GetColumns() const { return M; }

	//Получить i-строку
	inline double*& operator[] (size_t i);

	//Скопировать матрицу
	void operator=(const Matrix& Source);

	//Правильное удаление матрицы
	~Matrix()
	{
		for (size_t i = 0; i < M; ++i)
		{
			delete[] Ptr[i];
		}

		delete[] Ptr;
	}

	//Перевод матрицы в строку
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

	//Создать единичную матрицу
	static Matrix Identity(size_t n);
};

inline double*& Matrix::operator[](size_t i)
{
	return Ptr[i];
}