#pragma once
#define _USE_MATH_DEFINES
#include<math.h>

class Vector3f
{
public:
	float x, y, z;

	Vector3f() {};								//����������� �� ���������
	Vector3f(float x_0, float y_0, float z_0)	//�����������
	{
		x = x_0;
		y = y_0;
		z = z_0;
	}

	Vector3f operator + (Vector3f vect)
	{
		return Vector3f(vect.x + x, vect.y + y, vect.z + z);
	}

	Vector3f operator - (Vector3f vect)
	{
		return Vector3f(x - vect.x, y - vect.y, z - vect.z);
	}


	Vector3f operator * (float n)
	{
		return Vector3f(x * n, y * n, z * n);
	}


	Vector3f operator / (float n)
	{
		return Vector3f(x / n, y / n, z / n);
	}
};

//������� ���� ��������
Vector3f normal(Vector3f vect_1, Vector3f vect_2);

//����� �������
float norma(Vector3f vect);

//������������ �������
Vector3f normalize(Vector3f vect);

//��������� ������������ ���� ��������
float scalar(Vector3f vect_1, Vector3f vect_2);

//������ ����� 2 �������
Vector3f count_vect(Vector3f point_1, Vector3f point_2);

//������� ��� ��������
Vector3f count_normal_for_polygon(Vector3f one, Vector3f two, Vector3f three);