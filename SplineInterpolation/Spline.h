#pragma once

#include <vector>

//��������� ������ ��� ����������� �������� ������ �� ����� �������� �������
struct Segment
{
	Segment(double Begin, double End) :
		Begin(Begin), End(End), a(0), b(0), c(0), d(0) {}

	//�-���������� ������ 
	double Begin;

	//�-���������� �����
	double End;

	//����� ��������
	double Length;

	//������������ ����������� �������� ������� ��������
	double a, b, c, d;

	//�������� �� �������������� ����� � ������� ��������
	bool HasPoint(double x) { return x >= Begin && x <= End; }
};

//��������� ��� ����������� �������� ������ �������� �������
class Function
{
	//������ ��� �������� �������
	std::vector<std::pair<double, double>> Points;

public:

	//���� � ����� � ��������� Filename
	Function(const std::string& Filename);

	//i-�������� � �������
	double x(size_t i) const;

	//i-�������� � �������
	double y(size_t i) const;

	//��� i-�������� �������
	double Step(size_t i) const;

	//���������� ��� ��������
	size_t Size() const { return Points.size(); }
};

//��������� ��� ������ � ���������������� ���������
class IInterpolate
{

protected:

	//��������������� �������
	Function Func;

public:
	IInterpolate(Function InterpFunc) :
		Func(InterpFunc) {}

	virtual double operator()(double x) = 0;
};

//��������� ��� �������� ������ � �������
class CubicSpline : public IInterpolate
{
	//������ ��������� �������
	std::vector<Segment> Segments;

public:

	CubicSpline(Function Func);

	//���������� �������� ������-������� � ����� �
	double operator()(double x) override;

	//����� PointsCount ��� �������� �-� � ���� Filename
	void SerializeSplineData(const std::string& Filename, size_t PointsCount = 100);
};

