#pragma once
#include <vector>

/*
	* Класс трехдиагональной матрицы
	* Из прошлой лабораторной работы
	* Имеет минимальный интерфейс, достаточный для расчёта коэффициентов сплайна
*/

class DiagonalMatrix
{
	//Оператор ввода через стандартный поток
	friend std::istream& operator>>(std::istream&, DiagonalMatrix&);

	typedef std::vector<double> Vector;

	//Вектора коэффициентов
	//Вектор равенства (?)
	Vector r;

	//Нижняя, главная и верхняя диагонали
	Vector b;
	Vector c;
	Vector d;

	//Размер матрицы
	size_t n;

public:

	//Специальные константы, обозначающие диагонали. Для удобства.
	static const char TOP_DIAGONAL = 66;
	static const char MAIN_DIAGONAL = 67;
	static const char BOTTOM_DIAGONAL = 68;
	static const char SOLUTION = 69;


	DiagonalMatrix(size_t N) : n(N)
	{
		r.resize(n);
		c.resize(n);
		b.resize(n);
		d.resize(n);
	}

	//Найти решение
	Vector GetPlot() const;

	/*
		* Получить элемент из матрицы
		* line - обозначение диагонали (используются объявленные ранее константы)
		* idx - позиция
	*/
	double& operator()(char line, size_t idx);
};
