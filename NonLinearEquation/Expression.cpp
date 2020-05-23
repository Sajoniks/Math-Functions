#include "Expression.h"
#include <algorithm>
#include <cmath>

double neg(double x)
{
	return -x;
}

//����� �������, ������� ����� �������������� � ����������
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

//����� ��������
std::map<std::string, double> ConstMap =
{
	{"pi", (double)245850922 / 78256779},
	{"e", std::exp(1)}
};

//��������� �������������� ��������� �� ������ (�������� "������������� �������" � �������������)
Expression ExpressionBuilder::BuildExpression(const std::string & Expr)
{
	//���������, ���������� � �������� �������� �������
	std::queue<Token*> Output;
	//���� ����������
	std::stack<Token*> Stack;
	//���������� � ���������
	std::map<std::string, double> Vars;

	std::string Result = Expr;
	//������� �������
	Result.erase(std::remove_if(Result.begin(), Result.end(), isspace), Result.end());

	for (auto It = Result.cbegin(); It < Result.cend(); ++It)
	{
		//������� ������
		char Ch = *It;


		//���� �����, ��������� � �����
		if (isdigit(Ch))
		{
			//������� �������
			auto EndIt = std::find_if(It, Result.cend(), [](char Ch) {
				return !(isdigit(Ch)) && Ch != '.';
			});

			std::string Value;
			//�������� �����
			std::copy(It, EndIt, std::back_inserter(Value));
			It = EndIt - 1;

			//��������� � �����
			double Val = atof(Value.c_str());
			//���������� � �������� �������
			Output.push(new Number(Val));

			continue;
		}

		//���� �����, �� ��� ����� ���� ����������/���������/�������
		if (isalpha(Ch))
		{
			//������� �������
			auto EndIt = std::find_if(It, Result.cend(), [](char Ch) {
				return !isalpha(Ch);
			});

			std::string Value;
			//�������� �����
			std::copy(It, EndIt, std::back_inserter(Value));
			It = EndIt - 1;

			//���� ��������� �������, ������� � � ���� ����������
			//���� ��������� ���������, ������� � � �������� �������
			//����� ��� ����������, ��������� �� � ������ ���������� � � �������� �������
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

		//�������� ��� �����������
		switch (Ch)
		{
		//����������� ���������� � �������
		case ',':
		{
			while (!Stack.empty() && Stack.top()->GetSymbol() != "(")
			{
				Output.push(Stack.top());
				Stack.pop();
			}
			break;
		}
		//�������� 
		case '+': case '-': case '/': case '*': case '^':
		{
			//���� ��� �����, �� � ������ ������������� ���������� ��� � �������� ���������
			if (!Stack.empty() && Ch == '-' && (Stack.top()->GetSymbol() == "(" || Stack.top()->GetSymbol() == "-" || Stack.top()->GetSymbol() =="#"))
			{
				Stack.push(new Function("#"));
				continue;
			}

			//���������
			int Precedence = 4;
			//�������������������
			bool bLeft = true;

			//	+ - ��������� 2
			//	/ * ��������� 3
			//	^   ��������� 4

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

			//���� � ������� ��� �����, ������� ���� ����
			if (Output.empty())
				Output.push(new Number(0));
			//� � ���� ���������� ������� ����� ��������
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

	//������� ��������� � ���������� �������� � �����������
	return Expression(std::move(Output), std::move(Vars));
}

Variable::Variable(const std::string & Name):
		Name(Name) {}

void Function::Calculate(std::stack<double>& ValueStack) const
{
	//����� �������� � �������� ������ �������
	double x = ValueStack.top();
	ValueStack.pop();
	ValueStack.push(FunctionMap[Name](x));
}

void Operator::Calculate(std::stack<double>& ValueStack) const
{
	//����� �������� � �������� ������ ����������
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
	{//������� �������� ���������

	//���� ���������
		std::stack<double> ExpressionStack;
		//����� �������
		std::queue<Token*> Queue = RPN;

		while (!Queue.empty())
		{
			//���� �����, ����������� ��� � �������
			Number* Num = dynamic_cast<Number*>(Queue.front());
			if (Num)
			{
				ExpressionStack.push(Num->Get());
			}

			//���� ����������, ����������� � �������� � �������
			Variable* Var = dynamic_cast<Variable*>(Queue.front());
			if (Var)
			{
				ExpressionStack.push(Vars.at(Var->GetName()));
			}

			//���� ��������, ��� ��� �������� ������
			Operator* Op = dynamic_cast<Operator*>(Queue.front());
			if (Op)
			{
				Op->Calculate(ExpressionStack);
			}

			//���� �������, �� ������� � ��������
			Function* Func = dynamic_cast<Function*>(Queue.front());
			if (Func)
			{
				Func->Calculate(ExpressionStack);
			}

			//������� �����
			Queue.pop();
		}

		//� ������� ����� �������� �������� ���������
		Value = ExpressionStack.top();
	}
	
	return Value;
}

//������� �������� ��������� � ����� Point
double Expression::GetValue(const std::vector<double>& Point)
{
	for (size_t i = 0; i < GetVariablesCount(); ++i)
	{
		ChangeVariable(GetVariable(i), Point[i]);
	}

	return GetValue();
}

//idx-���������� ���������
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

//���������� ���������� � ���������
size_t Expression::GetVariablesCount() const
{
	return Vars.size();
}

//������ ����������� ��������� � ����� �� ���������� VarName (���� ��������� ����� ����������)
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

//������� ����������� ��������� � ����� �� ���������� VarName
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

//������� ����������� ��������� � ����� �� i-����������
double Expression::GetPartialDerivation(const std::vector<double>& Point, size_t Var)
{
	return GetPartialDerivation(Point, GetVariable(Var-1));
}

//���������� �������� ���������� VarName ��� ����� ��������� 
void Expression::ChangeVariable(const std::string& VarName, double Value)
{
	Vars[VarName] = Value;
	bDirty = true;
}


//���������� ������� ��������� ���������
void SetConstant(const std::string& Const, double Value)
{
	ConstMap[Const] = Value;
}
