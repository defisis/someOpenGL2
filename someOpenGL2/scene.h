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
Описание интерфейсов классов описывающих сцену.
*/
using namespace std;

//=============Для преобразования луча к базовому виду==============

// Определить матрицу трансформирования.
void makeTransformMatrix(double **TMatrix, Vector3f shift, GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ);

// Перевод луча к базовому виду.
Ray transformRay(Ray _ray, Vector3f shift, GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ);

// Скалярное произведение.
double scal(Vector3f vec1, Vector3f vec2);

// Определить позицию луча(точку соударения луча с объектом).
Vector3f rayPos(Ray ray, double t);

//==================================================================

// Описание интерфейса класса, описывающего цвет.
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

// Описание интерфейса класса, описывающего источник света.
class Light {
public:
	Vector3f position;				// Позиция источника света.
	Color color;					// Цвет источника света.

	Light();
	void setPosition(Vector3f pos); // Установить позицию источника света.
	void setColor(Color col);		// Установить цвет источника света.
};

// Описание интерфейса класса, описывающего объект сцены: сфера.
class Sphere {
public:
	GLfloat radius;			// Радиус сферы.
	Vector3f center_coord;	// Координаты центра сферы.
	Color color;			// Цвет сферы.
	Color FrameColor;		// Цвет каркаса сферы.
	bool display;			// true - если сфера отрисовывается.

	// Свойства материала сферы.
	GLfloat Ambient[4];		// Фоновое отражение.
	GLfloat Diffuse[4];		// Рассеянное отражение.
	GLfloat Specular[4];	// Зеркальное отражение.
	GLfloat Emission[4];	// Собственное излучение.
	GLfloat Shininess;		// Коэффициент блеска.

	Sphere();
	Sphere(
		GLfloat _radius, 
		Vector3f _center, 
		Color _color
	);
	void Draw();											 // Отрисовка сферы.
	void DrawFrame();										 // Отрисовка каркаса сферы.

	bool hit(Ray ray, Intersection &inter); // Определение соударений луча со сферой.
	bool hit(Ray ray);						// Упрощенный метод hit (не строит запись о пересечениях, используется для определения тени).
};

// Описание интерфейса класса, описывающего объект сцены: тетраэдр
class Tetrahedron {
public:
	Vector3f coord[4];		// Координаты вершин.
	Vector3f center_coord;	// Координаты центра тетраэдра.
	Color color;			// Цвет тетраэдра.
	Color FrameColor;		// Цвет каркаса тетраэдра.
	bool display;			// true - если тетраэдр отрисовывается.

							// Свойства материала тетраэдра.
	GLfloat Ambient[4];		// Фоновое отражение.
	GLfloat Diffuse[4];		// Рассеянное отражение.
	GLfloat Specular[4];	// Зеркальное отражение.
	GLfloat Emission[4];	// Собственное излучение.
	GLfloat Shininess;		// Коэффициент блеска.

	Tetrahedron();
	Tetrahedron(
		Vector3f _coord[], 
		Vector3f _center, 
		Color _color
	);
	void Draw();							// Отрисовка тетраэдра.
	void DrawFrame();						// Отрисовка каркаса тетраэдра.

	bool hit(Ray ray, Intersection &inter); // Определение соударений луча с тетраэдром.
	bool hit(Ray ray);						// Упрощенный метод hit (не строит запись о пересечениях, используется для определения тени).

	bool hitPlane(Ray ray, int ver1, int ver2, int ver3, Vector3f &N, float &time); // Определение соударения с гранью тетраэдра.
};

// Описание интерфейса класса, описывающего объект сцены: квадрат
class Square {
public:
	Vector3f center_coord;	// Координаты центра плоскости.
	GLfloat scaleX;			// Расстяжение по оси Х.
	GLfloat scaleZ;			// Расстяжение по оси Z.
	Color color;			// Цвет квадрата.

	// Свойства материала квадрата.
	GLfloat Ambient[4];		// Фоновое отражение.
	GLfloat Diffuse[4];		// Рассеянное отражение.
	GLfloat Specular[4];	// Зеркальное отражение.
	GLfloat Emission[4];	// Собственное излучение.
	GLfloat Shininess;		// Коэффициент блеска.

	Square();
	Square(Vector3f _center, GLfloat _scaleX, GLfloat _scaleZ, Color _color);
	void Draw();															  // Отрисовка квадрата.
	bool hit(Ray ray, Intersection &inter);									  // Определение соударений луча с квадратом.
	bool hit(Ray ray);														  // Упрощенный метод hit (не строит запись о пересечениях, используется для определения тени).
};

// Описание интерфейса класс сцены.
class Scene {
public:
	Square square;							// Плоскость.
	vector<Sphere> vector_Sphere;			// Контейнер сфер.
	vector<Tetrahedron> vector_Tetrahedron; // Контейнер тетраэдров.
	vector<Light> vector_Light;				// Контейнер источников освещения.

	Scene();
	void set_data();		// Считать из файла координаты фигур и их параметры.
	void switch_forward();  // Переключение между фигурами вперед.
	void switch_backward(); // Переключение между фигурами назад.

	void Draw();			  // Отрисовка сцены.
	bool isInShadow(Ray ray); // Находится ли объект в тени другого объекта.

	bool add_del;			// Режим включения/исключения объектов.
	bool sphere_mod;		// Переключение между объектами сферами.
	bool tetrahedron_mod;   // Переключение между объектами тетраэдрами.
	int active_Sphere;		// Активная сфера.
	int active_Tetrahedron; // Активный тетраэдр.
};
