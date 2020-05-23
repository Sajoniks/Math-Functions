// Integral.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include "Expression.h"
#include <thread>
#include <mutex>
#include <chrono>
#include "Utils.h"

std::mutex mutex;

using namespace Math;

//Количество итерация для метода Ньютона
constexpr size_t NEWTON_PRECISION = 100;

//Шаг для метода Ньютона
constexpr size_t STEP_SIZE = 50;

//Количество разбиений для метода прямоугольников
constexpr size_t PRECISION = 30;

//Структура для компактного хранения данных об интеграле
struct TaskData
{
	//Подинтегральная функция
	Expression E;
	
	//Верхний предел, чему равно
	double A{}, B{};
	
	//Начальное приближение (не используется)
	double x0{};
	
	//Начало и конец отрезка поиска корней
	double Start{}, End{};
};

//Структура для компактного хранения данных о корне
struct Root
{
	//Значение
	double Value = 0.0;
	//Период
	double Period = 0.0;

	bool operator==(const Root& r) const { return NearlyEqual(Value, r.Value) && NearlyEqual(Period, r.Period); }
	bool operator==(const double x) const { return NearlyEqual(Value, x); }
	bool operator<(const Root& r) const { return Value < r.Value; }
	operator double() const { return Value; }
};

//Рассчитать численное значение интеграла по формуле прямоугольников
double CalculateIntegral(Expression& E, const double A, const double B)
{
	//Шаг
	const double step = (B - A) / PRECISION;

	double fx = 0;
	for (size_t i = 0; i <= PRECISION; ++i)
	{
		const double point = (A + i * step) - 0.5 * step;
		
		//Т.к. E - подинтегральная функция, то не нужно искать производную
		fx += E({ point })*step;
	}

	return fx;
}

void ProcessX(const double Start, const double End, TaskData Data, std::vector<Root>& Roots)
{
	double x = Start;
	const double LocalityEps = 0.1;
	const double step = (Data.End - Data.Start) / (double)STEP_SIZE;
	
	for (size_t i = 0; i < 3; ++i)
	{
		//Количество совпадений для рассчета шага
		size_t EqualCount = 0;
		
		//Пока Х находится в пределах отрезка
		while (InRange(x, Start, End))
		{
			//Число, на которое нужно смещать функцию
			const double Delta = (i > 0 ? std::pow(-1, i)*LocalityEps : 0);
			
			//Значение на левом конце отрезка
			const double LeftFX = CalculateIntegral(Data.E, Data.A, x) - Data.B + Delta;

			//И на правом
			double RightFX = LeftFX;

			//Пока функция не поменяет знак и пока мы не выйдем за пределы отрезка - шагаем вправо
			while (LeftFX * RightFX >= 0 && InRange(x, Data.Start, Data.End))
			{
				x += step;
				RightFX = CalculateIntegral(Data.E, Data.A, x) - Data.B + Delta;
			}

			//Функция поменяла знак
			if (LeftFX * RightFX < 0)
			{
				//Найдены границы отрезка, где есть потенциальный корень
				//Количество итераций метода Ньютона
				size_t Iteration = 0;

				//Если значение f(x)*f``(x) > 0, то последовательность x_k начиная с x_0 будет сходится к корню уравенения
				//const double FX0 = LeftFX * Data.E.GetDerivation(StartX, "x");
				//double x0 = (FX0 > 0 ? x : StartX);

				double x0 = x;

				//Метод Ньютона
				//Используется обычный метод Ньютона
				double FX = CalculateIntegral(Data.E, Data.A, x0);
				double DFX = Data.E({ x0 });

				double xn = x0 + (Data.B - FX) / DFX;
				while (abs(x0 - xn) > SMALL_NUMBER && Iteration++ < NEWTON_PRECISION)
				{
					x0 = xn;
					FX = CalculateIntegral(Data.E, Data.A, x0);

					//Условие окончания итераций - малое приращение Х и почти равенство функции данному числу
					if (NearlyEqual(FX, Data.B, 0.005))
						break;

					DFX = Data.E({ x0 });

					xn += (Data.B - FX) / DFX;
				}

				x = xn + step;

				//Если корень в промежутке и он подходит для функции
				if (InRange(xn, Data.Start, Data.End) && NearlyEqual(FX, Data.B, 0.005))
				{
					std::lock_guard<std::mutex> Guard(mutex);
					
					//И если такого корня не было 
					auto It = std::find_if(Roots.begin(), Roots.end(), [&xn](Root& r)
					{
						return NearlyEqual(xn, r.Value, 1.0e-1);
					});

					if (It == Roots.end())
					{
						//Добавляем его в вектор решений
						Roots.push_back({ xn, 0 });
						EqualCount = 0;
					}
					else
					{
						//Иначе отступаем еще дальше
						EqualCount++;
						x += step * EqualCount;
					}
				}
			}
		}
		//Обнулили Х в начало, считаем с другим смещением функции
		x = Data.Start;
	}
}

std::vector<Root> SolveIntegralEquation(TaskData& Data, bool bMulti = true)
{
	std::vector<Root> s;

	if (bMulti)
	{
		std::vector<std::thread> Tasks;

		const size_t ConcurrencyVal = std::thread::hardware_concurrency();
		const size_t MaxCount = 4;
		const size_t ThreadsCount = std::min(ConcurrencyVal, MaxCount);


		if (ThreadsCount > 0)
		{
			const double Section = (Data.End - Data.Start) / ThreadsCount;

			for (size_t i = 0; i < ThreadsCount; ++i)
			{
				const double Start = Data.Start + Section * i;
				const double End = Data.Start + Section * (i + 1);
				Tasks.emplace_back(ProcessX, Start, End, Data, std::ref(s));
			}

			std::for_each(Tasks.begin(), Tasks.end(), [](std::thread& t)
			{
				t.join();
			});
		}
		else
		{
			ProcessX(Data.Start, Data.End, Data, s);
		}
	}
	else
	{
		ProcessX(Data.Start, Data.End, Data, s);
	}

	//Если не найдено ни одного корня, уведомляем
	if (s.empty())
	{
		char Buffer[1024];
		std::snprintf(Buffer, sizeof(Buffer), R"(No solution found for: "int from %f to x (%s) = %f")", Data.A, Data.E.ToString().c_str(), Data.B);
		throw std::runtime_error(Buffer);
	}

	std::sort(s.begin(), s.end(), [](Root& left, Root& right)
	{
		return left.Value < right.Value;
	});

	return s;
}

//Прочесть с файла
TaskData ReadFromFile(std::ifstream& File)
{
	TaskData Data;

	if (File)
	{
		File >> Data.A >> Data.B >> Data.Start >> Data.End;
		File.ignore();
		std::string Expression;
		std::getline(File, Expression);
		Data.E = ExpressionBuilder::BuildExpression(Expression);
	}

	return Data;
}

void WriteSpeedTest(TaskData& d)
{
	auto StartTime = std::chrono::high_resolution_clock::now();
	SolveIntegralEquation(d, false);
	auto EndTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> Duration = EndTime - StartTime;

	std::cout << "Not-multithreaded integral solver took " << Duration.count() / 1000.f << " seconds.\n";

	StartTime = std::chrono::high_resolution_clock::now();
	SolveIntegralEquation(d, true);
	EndTime = std::chrono::high_resolution_clock::now();
	Duration = EndTime - StartTime;

	std::cout << "Multithreaded integral solver took " << Duration.count() / 1000.f << " seconds.\n";
}

//Построить таблицу значений подинтегральной функции
void PlotIntegral(TaskData& Task, const std::string& Filename)
{
	std::ofstream f(Filename);
	
	double s = (Task.End - Task.Start) / 100;
	for (size_t i = 0; i <= 100; ++i)
	{
		const double x = Task.Start + s * i;
		const double y = CalculateIntegral(Task.E, Task.A, x);
		
		f << x << '\t' << y << '\n';
	}
}

int main()
{
	TaskData d;
	for (size_t i = 1; i <= 4; ++i)
	{
		char Buffer[9];
		std::snprintf(Buffer, 9, "in%zd.txt", i);
		std::ifstream file(Buffer);

		if (file)
		{
			std::snprintf(Buffer, 9, "out%zd.txt", i);
			std::ofstream out(Buffer);

			try
			{
				char Name[30];
				snprintf(Name, sizeof(Name), "plot%zd.txt", i);

				d = ReadFromFile(file);
				PlotIntegral(d, Name);

				auto x = SolveIntegralEquation(d);

				out << "x = ( ";
				std::cout << "x = ( ";

				std::for_each(x.begin(), x.end(), [&out](Root& r)
				{
					out << r.Value << ' ';
					std::cout << r.Value << ' ';

					if (r.Period > 0)
					{
						out << "+ " << r.Period << "k ";
						std::cout << "+ " << r.Period << "k ";
					}
				});

				out << ");";
				std::cout << ");\n";

				//WriteSpeedTest(d);
			}
			catch (std::exception& e)
			{
				out << e.what();
				std::cout << e.what() << " in file: in" << i << ".txt\n";

				out.close();
				file.close();
			}
		}
	}
	return 0;
}