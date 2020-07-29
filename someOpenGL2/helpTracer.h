/*
Описание вспомогательных классов.
*/

#include "camera.h"

// Интерфейс класса "луч".
class Ray {
public:
	Vector3f start;		// Стартовая точка луча.
	Vector3f dir;		// Направление луча.
	int recurseLevel;	// Текущий уровень рекурсии.

	Ray();
	void SetStart(Vector3f _start);	// Установить стартовую точку.
	void SetDir(Vector3f _dir);		// Установить направление.
};

// Интерфейс класса описывающего информацию о соударении.
class HitInfo {
public:
	double hitTime;		// Время соударения.
	int objectType;		// Тип объекта соударения.
	int objectNum;		// Номер объекта соударения.
	int surface;		// Поверхность соударения.
	bool isEntering;	// ?Луч входит или выходит из объекта.

	Vector3f hitPoint;	// Точка соударения.
	Vector3f hitNormal;	// Нормаль в точке соударения.

	HitInfo();
	void set(HitInfo hI);
};

// Интерфейс класса описывающего список соударений.
class Intersection {
public:
#define maxNumHits 8
	int numHits;				// Число соударений для положительных значений времени.
	HitInfo hit[maxNumHits];	// Список соударений.

	Intersection();
	void set(Intersection intr);
};