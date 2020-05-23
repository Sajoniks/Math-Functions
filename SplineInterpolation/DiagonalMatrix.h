#pragma once
#include <vector>

/*
	* ����� ���������������� �������
	* �� ������� ������������ ������
	* ����� ����������� ���������, ����������� ��� ������� ������������� �������
*/

class DiagonalMatrix
{
	//�������� ����� ����� ����������� �����
	friend std::istream& operator>>(std::istream&, DiagonalMatrix&);

	typedef std::vector<double> Vector;

	//������� �������������
	//������ ��������� (?)
	Vector r;

	//������, ������� � ������� ���������
	Vector b;
	Vector c;
	Vector d;

	//������ �������
	size_t n;

public:

	//����������� ���������, ������������ ���������. ��� ��������.
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

	//����� �������
	Vector GetPlot() const;

	/*
		* �������� ������� �� �������
		* line - ����������� ��������� (������������ ����������� ����� ���������)
		* idx - �������
	*/
	double& operator()(char line, size_t idx);
};
