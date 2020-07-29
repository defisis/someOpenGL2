/*
Описание интерфейса класса камеры
*/

#include <math.h>
#include <windows.h>
#include "glut.h" 
#include <vector>
#include <gl\gl.h>
#include "normals.h"


class Camera {
public:
	Vector3f Position;	// Позиция камеры/положение точки наблюдателя.
	Vector3f View;		// Направление наблюдения.
	Vector3f UpVector;	// Вектор поворота сцены.

	Camera();
	void PositionCamera(float posX, float posY, float posZ,
		float viewX, float viewY, float viewZ,
		float upX, float upY, float upZ);	// Установка позиции камеры.

	void SetViewByMouse(GLint width, GLint height);				// Установка вида с помощью мыши.
	void MoveCamera(float speed);								// Передвижение камеры вперед/назад.
	void RotateView(float angle, float x, float y, float z);	// Вращение камеры вокруг заданной оси.
	void RotateAroundPoint(Vector3f vCenter, float angle, float x, float y, float z);	// Вращение камеры вокруг наблюдателя.
};