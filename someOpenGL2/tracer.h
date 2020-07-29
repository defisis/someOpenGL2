/*
��������� ������, ����������� �� ����������� �����.
*/

#include "scene.h"

class Raytracer {
public:
	Vector3f eye;	// ����� ������ �����/��������� ������.
	Scene scene;	// �����.
	Camera camera;	// ������.
	float aspect;
	int blockSize;	// ������ ����� ��������.
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

	void Raytrace();		// �����������.
	Color Shade(Ray ray);	// ���� �����������.
	Intersection getFirstHit(Ray ray);	// �����, ��� ���������� ������� ����������� �� ���� ���������� ����.
};