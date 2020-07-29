#define _CRT_SECURE_NO_WARNINGS
#include <math.h>
#include <windows.h>
#include "glut.h"
#include <vector>
#include <gl\gl.h>
#include <stdio.h>
#include "helpTracer.h"
#include "gauss.h"

/*
�������� ����������� ������� ����������� �����.
*/
using namespace std;

//=============��� �������������� ���� � �������� ����==============

// ���������� ������� �����������������.
void makeTransformMatrix(double **TMatrix, Vector3f shift, GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ);

// ������� ���� � �������� ����.
Ray transformRay(Ray _ray, Vector3f shift, GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ);

// ��������� ������������.
double scal(Vector3f vec1, Vector3f vec2);

// ���������� ������� ����(����� ���������� ���� � ��������).
Vector3f rayPos(Ray ray, double t);

//==================================================================

// �������� ���������� ������, ������������ ����.
class Color {
public:
	GLfloat red;
	GLfloat green;
	GLfloat blue;

	Color();
	Color(
		GLfloat _red, 
		GLfloat _green, 
		GLfloat _blue
	);
	void add(
		GLfloat _red, 
		GLfloat _green,
		GLfloat _blue)
		;
	void add(Color colr);
	void add(Color colr, Color refl);
};

// �������� ���������� ������, ������������ �������� �����.
class Light {
public:
	Vector3f position;				// ������� ��������� �����.
	Color color;					// ���� ��������� �����.

	Light();
	void setPosition(Vector3f pos); // ���������� ������� ��������� �����.
	void setColor(Color col);		// ���������� ���� ��������� �����.
};

// �������� ���������� ������, ������������ ������ �����: �����.
class Sphere {
public:
	GLfloat radius;			// ������ �����.
	Vector3f center_coord;	// ���������� ������ �����.
	Color color;			// ���� �����.
	Color FrameColor;		// ���� ������� �����.
	bool display;			// true - ���� ����� ��������������.

	// �������� ��������� �����.
	GLfloat Ambient[4];		// ������� ���������.
	GLfloat Diffuse[4];		// ���������� ���������.
	GLfloat Specular[4];	// ���������� ���������.
	GLfloat Emission[4];	// ����������� ���������.
	GLfloat Shininess;		// ����������� ������.

	Sphere();
	Sphere(
		GLfloat _radius, 
		Vector3f _center, 
		Color _color
	);
	void Draw();											 // ��������� �����.
	void DrawFrame();										 // ��������� ������� �����.

	bool hit(Ray ray, Intersection &inter); // ����������� ���������� ���� �� ������.
	bool hit(Ray ray);						// ���������� ����� hit (�� ������ ������ � ������������, ������������ ��� ����������� ����).
};

// �������� ���������� ������, ������������ ������ �����: ��������
class Tetrahedron {
public:
	Vector3f coord[4];		// ���������� ������.
	Vector3f center_coord;	// ���������� ������ ���������.
	Color color;			// ���� ���������.
	Color FrameColor;		// ���� ������� ���������.
	bool display;			// true - ���� �������� ��������������.

							// �������� ��������� ���������.
	GLfloat Ambient[4];		// ������� ���������.
	GLfloat Diffuse[4];		// ���������� ���������.
	GLfloat Specular[4];	// ���������� ���������.
	GLfloat Emission[4];	// ����������� ���������.
	GLfloat Shininess;		// ����������� ������.

	Tetrahedron();
	Tetrahedron(
		Vector3f _coord[], 
		Vector3f _center, 
		Color _color
	);
	void Draw();							// ��������� ���������.
	void DrawFrame();						// ��������� ������� ���������.

	bool hit(Ray ray, Intersection &inter); // ����������� ���������� ���� � ����������.
	bool hit(Ray ray);						// ���������� ����� hit (�� ������ ������ � ������������, ������������ ��� ����������� ����).

	bool hitPlane(Ray ray, int ver1, int ver2, int ver3, Vector3f &N, float &time); // ����������� ���������� � ������ ���������.
};

// �������� ���������� ������, ������������ ������ �����: �������
class Square {
public:
	Vector3f center_coord;	// ���������� ������ ���������.
	GLfloat scaleX;			// ����������� �� ��� �.
	GLfloat scaleZ;			// ����������� �� ��� Z.
	Color color;			// ���� ��������.

	// �������� ��������� ��������.
	GLfloat Ambient[4];		// ������� ���������.
	GLfloat Diffuse[4];		// ���������� ���������.
	GLfloat Specular[4];	// ���������� ���������.
	GLfloat Emission[4];	// ����������� ���������.
	GLfloat Shininess;		// ����������� ������.

	Square();
	Square(Vector3f _center, GLfloat _scaleX, GLfloat _scaleZ, Color _color);
	void Draw();															  // ��������� ��������.
	bool hit(Ray ray, Intersection &inter);									  // ����������� ���������� ���� � ���������.
	bool hit(Ray ray);														  // ���������� ����� hit (�� ������ ������ � ������������, ������������ ��� ����������� ����).
};

// �������� ���������� ����� �����.
class Scene {
public:
	Square square;							// ���������.
	vector<Sphere> vector_Sphere;			// ��������� ����.
	vector<Tetrahedron> vector_Tetrahedron; // ��������� ����������.
	vector<Light> vector_Light;				// ��������� ���������� ���������.

	Scene();
	void set_data();		// ������� �� ����� ���������� ����� � �� ���������.
	void switch_forward();  // ������������ ����� �������� ������.
	void switch_backward(); // ������������ ����� �������� �����.

	void Draw();			  // ��������� �����.
	bool isInShadow(Ray ray); // ��������� �� ������ � ���� ������� �������.

	bool add_del;			// ����� ���������/���������� ��������.
	bool sphere_mod;		// ������������ ����� ��������� �������.
	bool tetrahedron_mod;   // ������������ ����� ��������� �����������.
	int active_Sphere;		// �������� �����.
	int active_Tetrahedron; // �������� ��������.
};
