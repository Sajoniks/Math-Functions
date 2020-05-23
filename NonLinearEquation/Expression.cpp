#include "Expression.h"
#include <algorithm>
#include <cmath>

double neg(double x)
{
	return -x;
}

//Набор функций, которые могут использоваться в выражениях
std::map<std::string, double(*)(double)> FunctionMap = 
{
	{"sin", std::sin},
	{"cos", std::cos},
	{"tan", std::tan},
	{"atan", std::atan},
	{"exp", std::exp},
	{"ln", std::log},
	{"sqrt", std::sqrt},
	{"#", neg}
};

//Набор констант
std::map<std::string, double> ConstMap =
{
	{"pi", (double)245850922 / 78256779},
	{"e", std::exp(1)}
};

//Построить математическое выражение из строки (алгоритм "Сортировочной станции" с модификациями)
Expression ExpressionBuilder::BuildExpression(const std::string & Expr)
{
	//Выражение, записанное в обратной польской нотации
	std::queue<Token*> Output;
	//Стек операторов
	std::stack<Token*> Stack;
	//Переменные в выражении
	std::map<std::string, double> Vars;

	std::string Result = Expr;
	//Удаляем пробелы
	Result.erase(std::remove_if(Result.begin(), Result.end(), isspace), Result.end());

	for (auto It = Result.cbegin(); It < Result.cend(); ++It)
	{
		//Текущий символ
		char Ch = *It;


		//Если цифра, переводим в число
		if (isdigit(Ch))
		{
			//Находим границу
			auto EndIt = std::find_if(It, Result.cend(), [](char Ch) {
				return !(isdigit(Ch)) && Ch != '.';
			});

			std::string Value;
			//Копируем цифру
			std::copy(It, EndIt, std::back_inserter(Value));
			It = EndIt - 1;

			//Переводим в число
			double Val = atof(Value.c_str());
			//Отправляем в выходную очередь
			Output.push(new Number(Val));

			continue;
		}

		//Если буква, то это может быть переменная/константа/функция
		if (isalpha(Ch))
		{
			//Находим границу
			auto EndIt = std::find_if(It, Result.cend(), [](char Ch) {
				return !isalpha(Ch);
			});

			std::string Value;
			//Копируем слово
			std::copy(It, EndIt, std::back_inserter(Value));
			It = EndIt - 1;

			//Если известная функция, заносим её в стек операторов
			//Если известная константа, заносим её в выходную очередь
			//Иначе это переменная, добавляем ее в список переменных и в выходную очередь
			if (FunctionMap.find(Value) != FunctionMap.end())
				Stack.push(new Function(Value));
			else if (ConstMap.find(Value) != ConstMap.end())
			{
				Output.push(new Number(ConstMap[Value]));
			}
			else
			{
				Output.push(new Variable(Value));
				Vars[Value] = 0;
			}

			continue;
		}

		//Оператор или разделитель
		switch (Ch)
		{
		//Разделитель аргументов в функции
		case ',':
		{
			while (!Stack.empty() && Stack.top()->GetSymbol() != "(")
			{
				Output.push(Stack.top());
				Stack.pop();
			}
			break;
		}
		//Оператор 
		case '+': case '-': case '/': case '*': case '^':
		{
			//Если это минус, то в случае необходимости превращаем его в оператор отрицания
			if (!Stack.empty() && Ch == '-' && (Stack.top()->GetSymbol() == "(" || Stack.top()->GetSymbol() == "-" || Stack.top()->GetSymbol() =="#"))
			{
				Stack.push(new Function("#"));
				continue;
			}

			//Приоритет
			int Precedence = 4;
			//Левоассоциативность
			bool bLeft = true;

			//	+ - приоритет 2
			//	/ * приоритет 3
			//	^   приоритет 4

			if (Ch == '+' || Ch == '-')
				Precedence = 2;
			if (Ch == '/' || Ch == '*')
				Precedence = 3;
			if (Ch == '^')
				bLeft = false;

			if (!Stack.empty())
			{
				Operator* Op = dynamic_cast<Operator*>(Stack.top());
				while (!Stack.empty() && Op && Op->GetPrecendence() >= Precedence)
				{
					Output.push(Stack.top());
					Stack.pop();

					if (!Stack.empty())
						Op = dynamic_cast<Operator*>(Stack.top());
				}
			}

			//Если в очереди нет чисел, положим туда ноль
			if (Output.empty())
				Output.push(new Number(0));
			//А в стек операторов положим новый оператор
			Stack.push(new Operator(Ch, Precedence, bLeft));

			break;
		}
		case ')':
			while (!Stack.empty() && Stack.top()->GetSymbol() != "(")
			{
				Output.push(Stack.top());
				Stack.pop();
			}
			Stack.pop();

			if (!Stack.empty() && dynamic_cast<Function*>(Stack.top()))
			{
				Output.push(Stack.top());
				Stack.pop();
			}
			break;
		case '(':
				Stack.push(new Operator(Ch, 0, false));
			break;
		}
	}

	while (!Stack.empty())
	{
		Output.push(Stack.top());
		Stack.pop();
	}

	//Создаем выражение с полученной очередью и переменными
	return Expression(std::move(Output), std::move(Vars));
}

Variable::Variable(const std::string & Name):
		Name(Name) {}

void Function::Calculate(std::stack<double>& ValueStack) const
{
	//Берем значение и проводим расчет функции
	double x = ValueStack.top();
	ValueStack.pop();
	ValueStack.push(FunctionMap[Name](x));
}

void Operator::Calculate(std::stack<double>& ValueStack) const
{
	//Берем значения и проводим расчет оператором
	double lval;
	double rval = ValueStack.top();
	ValueStack.pop();
	lval = ValueStack.top();
	ValueStack.pop();

	double r = 0;

 	switch (_Symbol[0])
	{
	case '+':
		r = lval + rval;
		break;
	case '-':
		r = lval - rval;
		break;
	case '*':
		r = lval * rval;
		break;
	case '/':
		r = lval / rval;
		break;
	case '^':
		r = std::pow(lval, rval);
		break;
	}

	ValueStack.push(r);
}

double Expression::GetValue()
{
	if (bDirty)
	{//Считаем значение выражения

	//Стек выражения
		std::stack<double> ExpressionStack;
		//Копия очереди
		std::queue<Token*> Queue = RPN;

		while (!Queue.empty())
		{
			//Если число, выталкиваем его в очередь
			Number* Num = dynamic_cast<Number*>(Queue.front());
			if (Num)
			{
				ExpressionStack.push(Num->Get());
			}

			//Если переменная, выталкиваем её значение в очередь
			Variable* Var = dynamic_cast<Variable*>(Queue.front());
			if (Var)
			{
				ExpressionStack.push(Vars.at(Var->GetName()));
			}

			//Если оператор, даём ему провести расчёт
			Operator* Op = dynamic_cast<Operator*>(Queue.front());
			if (Op)
			{
				Op->Calculate(ExpressionStack);
			}

			//Если функция, то считаем её значение
			Function* Func = dynamic_cast<Function*>(Queue.front());
			if (Func)
			{
				Func->Calculate(ExpressionStack);
			}

			//Удаляем токен
			Queue.pop();
		}

		//В вершине стека получаем значение выражения
		Value = ExpressionStack.top();
	}
	
	return Value;
}

//Считаем значение выражение в точке Point
double Expression::GetValue(const std::vector<double>& Point)
{
	for (size_t i = 0; i < GetVariablesCount(); ++i)
	{
		ChangeVariable(GetVariable(i), Point[i]);
	}

	return GetValue();
}

//idx-переменная выражения
std::string Expression::GetVariable(size_t idx) const
{
	if (GetVariablesCount() > 0)
	{
		auto It = Vars.begin();
		while (idx > 0 && It != Vars.end())
		{
			It++;
			--idx;
		}

		if (It != Vars.end())
			return It->first;
	}

	return "";
}

//Количество переменных в выражении
size_t Expression::GetVariablesCount() const
{
	return Vars.size();
}

//Первая производная выражения в точке по переменной VarName (если выражение одной переменной)
double Expression::GetDerivation(double x, const std::string& VarName)
{
	if (GetVariablesCount() == 1)
	{
		double dx = 0.0001;
		ChangeVariable(VarName, x + dx);
		double dfx = GetValue();
		ChangeVariable(VarName, x);
		double fx = GetValue();

		return (dfx - fx) / dx;
	}
	else
		return 0;
}

//Частная производная выражения в точке по переменной VarName
double Expression::GetPartialDerivation(const std::vector<double>& Point, const std::string& VarName)
{
	if (Point.size() == GetVariablesCount())
	{
		auto It = Vars.begin();
		for (size_t i = 0; i < Point.size(); ++i)
		{
			(It++)->second = Point[i];
		}
		double fx = GetValue();

		double dx = 0.0001;
		Vars[VarName] += dx;
		double dfx = GetValue();

		return (dfx - fx) / dx;
	}
	else
		return 0;
}

//Частная производная выражения в точке по i-переменной
double Expression::GetPartialDerivation(const std::vector<double>& Point, size_t Var)
{
	return GetPartialDerivation(Point, GetVariable(Var-1));
}

//Установить значение переменной VarName для этого выражения 
void Expression::ChangeVariable(const std::string& VarName, double Value)
{
	Vars[VarName] = Value;
	bDirty = true;
}


//Глобальная функция установки константы
void SetConstant(const std::string& Const, double Value)
{
	ConstMap[Const] = Value;
}
