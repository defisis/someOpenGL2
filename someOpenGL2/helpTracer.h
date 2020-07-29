/*
�������� ��������������� �������.
*/

#include "camera.h"

// ��������� ������ "���".
class Ray {
public:
	Vector3f start;		// ��������� ����� ����.
	Vector3f dir;		// ����������� ����.
	int recurseLevel;	// ������� ������� ��������.

	Ray();
	void SetStart(Vector3f _start);	// ���������� ��������� �����.
	void SetDir(Vector3f _dir);		// ���������� �����������.
};

// ��������� ������ ������������ ���������� � ����������.
class HitInfo {
public:
	double hitTime;		// ����� ����������.
	int objectType;		// ��� ������� ����������.
	int objectNum;		// ����� ������� ����������.
	int surface;		// ����������� ����������.
	bool isEntering;	// ?��� ������ ��� ������� �� �������.

	Vector3f hitPoint;	// ����� ����������.
	Vector3f hitNormal;	// ������� � ����� ����������.

	HitInfo();
	void set(HitInfo hI);
};

// ��������� ������ ������������ ������ ����������.
class Intersection {
public:
#define maxNumHits 8
	int numHits;				// ����� ���������� ��� ������������� �������� �������.
	HitInfo hit[maxNumHits];	// ������ ����������.

	Intersection();
	void set(Intersection intr);
};