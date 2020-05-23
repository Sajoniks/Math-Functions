#pragma once
#include <string>
#include <queue>
#include <map>
#include <stack>

//������� ��������� ���������
void SetConstant(const std::string& Const, double Value);

//�����, ������������ ����� ���������
class Token
{
public:
	virtual ~Token() = default;
	virtual std::string GetSymbol() const = 0;
};

//�����, ������������ �����
class Number : public Token
{
	double Value;

public:
	Number(double Value) :
		Value(Value) {}

	double Get() const { return Value; }
	std::string GetSymbol() const override { return std::to_string(Value); }
};

//�����, ������������ �������
class Function : public Token
{
	std::string Name;
public:
	Function(const std::string& Name):
		Name(Name) {}

	std::string GetSymbol() const override { return Name; }
	void Calculate(std::stack<double>& ValueStack) const;
};

//�����, ������������ ����������
class Variable : public Token
{
	std::string Name;
public:

	Variable(const std::string& Name);
	std::string GetSymbol() const override { return Name; }
	const std::string& GetName() const { return Name; }
};

//�����, ������������ ��������
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

//�����, ������������ �������������� ���������
class Expression
{
	//�������� ���������
	double Value;
	//������ ������������� ��������� �������� ���������
	bool bDirty = true;
	//������� � �������� �������� �������
	std::queue<Token*> RPN;
	//������� ���������� � ���������
	std::map<std::string, double> Vars;

	std::string StringRep;

public:
	
	//����������� �� ������� � �������
	Expression(std::queue<Token*>&& RPN, std::map<std::string, double> Vars = std::map<std::string, double>(), std::string Rep = std::string()) :
		Value(0), RPN(RPN), Vars(std::move(Vars)), StringRep(Rep) {}

	Expression():
		Value(0) {}

	const std::string& ToString() const;
	
	//������ �������� ���������
	double		GetValue();
	
	//������ �������� ��������� � �����
	double		GetValue(const std::vector<double>& Point);
	double operator()(const std::vector<double>& Point);
	
	//�������� ���������� � ������� idx
	std::string GetVariable(size_t idx) const;
	//���������� ���������� � ���������
	size_t		GetVariablesCount() const;
	//������ ����������� �� ���������� VarName � ����� (���� ������� ����� ����������)
	double		GetDerivation(double x, const std::string& VarName);
	//������� ����������� �� ���������� VarName � �����
	double		GetPartialDerivation(const std::vector<double>& Point, const std::string& VarName);
	//������� ����������� �� ���������� � ������� Var
	double		GetPartialDerivation(const std::vector<double>& Point, size_t Var);
	//���������� �������� ���������� ��� ����� ���������
	void		ChangeVariable(const std::string& VarName, double Value);
	//�������� �� �������
	bool		HasFunction(const std::string& FuncName) const;
};

//����� ������������ ���������
//��� ����, ���������� ����� ���������
class ExpressionBuilder
{
public:
	static Expression BuildExpression(const std::string& Expr);
};
