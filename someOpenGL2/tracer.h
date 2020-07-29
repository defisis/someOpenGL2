/*
Интерфейс класса, отвечающего за трассировку лучей.
*/

#include "scene.h"

class Raytracer {
public:
	Vector3f eye;	// Точка выхода лучей/положение камеры.
	Scene scene;	// Сцена.
	Camera camera;	// Камера.
	float aspect;
	int blockSize;	// Размер блока пикселей.
	int nCols;
	int nRows;

	Raytracer();
	Raytracer(
		Vector3f _eye,
		Scene _scene,
		Camera _camera,
		float _aspect,
		int _nCols,
		int _nRows,
		int _blockSize
	);

	void Raytrace();		// Трассировка.
	Color Shade(Ray ray);	// Ядро трассировки.
	Intersection getFirstHit(Ray ray);	// Метод, для нахождения первого препятствия на пути следования луча.
};