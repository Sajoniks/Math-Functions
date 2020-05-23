#pragma once

#include <vector>

//Структура данных для компактного хранения данных об одном сегменте сплайна
struct Segment
{
	Segment(double Begin, double End) :
		Begin(Begin), End(End), a(0), b(0), c(0), d(0) {}

	//Х-координата начала 
	double Begin;

	//Х-координата конца
	double End;

	//Длина сегмента
	double Length;

	//Коэффициенты кубического полинома данного сегмента
	double a, b, c, d;

	//Проверка на принадлежность точки Х данному сегменту
	bool HasPoint(double x) { return x >= Begin && x <= End; }
};

//Структура для компактного хранения данных вводимой функции
class Function
{
	//Массив пар значений функции
	std::vector<std::pair<double, double>> Points;

public:

	//Ввод с файла с названием Filename
	Function(const std::string& Filename);

	//i-значение Х функции
	double x(size_t i) const;

	//i-значение Х функции
	double y(size_t i) const;

	//Шаг i-значения функции
	double Step(size_t i) const;

	//Количество пар значений
	size_t Size() const { return Points.size(); }
};

//Интерфейс для работы с интерполирующими функциями
class IInterpolate
{

protected:

	//Интерполируемая функция
	Function Func;

public:
	IInterpolate(Function InterpFunc) :
		Func(InterpFunc) {}

	virtual double operator()(double x) = 0;
};

//Структура для хранения данных о сплайне
class CubicSpline : public IInterpolate
{
	//Массив сегментов сплайна
	std::vector<Segment> Segments;

public:

	CubicSpline(Function Func);

	//Рассчитать значение сплайн-функции в точке Х
	double operator()(double x) override;

	//Вывод PointsCount пар значений х-у в файл Filename
	void SerializeSplineData(const std::string& Filename, size_t PointsCount = 100);
};

