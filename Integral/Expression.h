#pragma once
#include <string>
#include <queue>
#include <map>
#include <stack>

//Функция установки константы
void SetConstant(const std::string& Const, double Value);

//Класс, определяющий часть выражения
class Token
{
public:
	virtual ~Token() = default;
	virtual std::string GetSymbol() const = 0;
};

//Класс, определяющий число
class Number : public Token
{
	double Value;

public:
	Number(double Value) :
		Value(Value) {}

	double Get() const { return Value; }
	std::string GetSymbol() const override { return std::to_string(Value); }
};

//Класс, определяющий функцию
class Function : public Token
{
	std::string Name;
public:
	Function(const std::string& Name):
		Name(Name) {}

	std::string GetSymbol() const override { return Name; }
	void Calculate(std::stack<double>& ValueStack) const;
};

//Класс, определяющий переменную
class Variable : public Token
{
	std::string Name;
public:

	Variable(const std::string& Name);
	std::string GetSymbol() const override { return Name; }
	const std::string& GetName() const { return Name; }
};

//Класс, определяющий оператор
class Operator : public Token
{
	std::string _Symbol;
	bool bLeft;
	int Precedence;

public:
	Operator(char Symbol, int Precedence, bool bLeftAssociative) :
		Precedence(Precedence), bLeft(bLeftAssociative) {
		_Symbol = Symbol;
	}

	std::string GetSymbol() const override { return _Symbol; }
	int GetPrecendence() const { return Precedence; }
	void Calculate(std::stack<double>& ValueStack) const;
};

//Класс, определяющий математическое выражение
class Expression
{
	//Значений выражения
	double Value;
	//Флажок необходимости пересчёта значения выражения
	bool bDirty = true;
	//Очередь в обратной польской нотации
	std::queue<Token*> RPN;
	//Словарь переменных в выражении
	std::map<std::string, double> Vars;

	std::string StringRep;

public:
	
	//Конструктор из очереди и словаря
	Expression(std::queue<Token*>&& RPN, std::map<std::string, double> Vars = std::map<std::string, double>(), std::string Rep = std::string()) :
		Value(0), RPN(RPN), Vars(std::move(Vars)), StringRep(Rep) {}

	Expression():
		Value(0) {}

	const std::string& ToString() const;
	
	//Расчёт значения выражения
	double		GetValue();
	
	//Расчёт значения выражения в точке
	double		GetValue(const std::vector<double>& Point);
	double operator()(const std::vector<double>& Point);
	
	//Получить переменную с номером idx
	std::string GetVariable(size_t idx) const;
	//Количество переменных в выражении
	size_t		GetVariablesCount() const;
	//Первая производная по переменной VarName в точке (если функция одной переменной)
	double		GetDerivation(double x, const std::string& VarName);
	//Частная производная по переменной VarName в точке
	double		GetPartialDerivation(const std::vector<double>& Point, const std::string& VarName);
	//Частная производная по переменной с номером Var
	double		GetPartialDerivation(const std::vector<double>& Point, size_t Var);
	//Установить значение переменной для этого выражения
	void		ChangeVariable(const std::string& VarName, double Value);
	//Содержит ли функцию
	bool		HasFunction(const std::string& FuncName) const;
};

//Класс конструктора выражения
//Для того, разгрузить класс выражения
class ExpressionBuilder
{
public:
	static Expression BuildExpression(const std::string& Expr);
};
