#include "camera.h"

/*
Реализация интерфейсов класса камеры.
*/
Camera::Camera() {
}
void Camera::PositionCamera(float posX, float posY, float posZ,
	float viewX, float viewY, float viewZ,
	float upX, float upY, float upZ) {
	// Установить позицию камеры.
	Vector3f _Position = Vector3f(
		posX, 
		posY, 
		posZ
	);
	Vector3f _View = Vector3f(
		viewX, 
		viewY, 
		viewZ
	);
	Vector3f _UpVector = Vector3f(
		upX, 
		upY, 
		upZ
	);

	Position = _Position;
	View = _View;
	UpVector = _UpVector;
}
void Camera::MoveCamera(float speed){
	Vector3f _View = View - Position; // Определить направление взгляда.
	// Передвижение камеры.
	Position.x += _View.x * speed; // Изменить положение.
	Position.z += _View.z * speed; // Камеры.

	View.x += _View.x * speed; // Изменить направление.
	View.z += _View.z * speed; // Взгляда камеры.
}
void Camera::RotateView(float angle, float x, float y, float z) {
	Vector3f _newView;
	Vector3f _View;

	// Определить направление взгляда.
	_View = View - Position;

	// Рассчитать синус и косинус переданного угла.
	float cosA = (float)cos(angle);
	float sinA = (float)sin(angle);

	/*	Пересчет координат по каким-то диким формулам
		новая координата X для вращаемой точки. */
	_newView.x = (cosA + (1 - cosA) * x * x) * _View.x;
	_newView.x += ((1 - cosA) * x * y - z * sinA) * _View.y;
	_newView.x += ((1 - cosA) * x * z + y * sinA) * _View.z;

	// Новая координата Y для вращаемой точки.
	_newView.y = ((1 - cosA) * x * y + z * sinA) * _View.x;
	_newView.y += (cosA + (1 - cosA) * y * y) * _View.y;
	_newView.y += ((1 - cosA) * y * z - x * sinA) * _View.z;

	// Новая координата Z для вращаемой точки.
	_newView.z = ((1 - cosA) * x * z - y * sinA) * _View.x;
	_newView.z += ((1 - cosA) * y * z + x * sinA) * _View.y;
	_newView.z += (cosA + (1 - cosA) * z * z) * _View.z;

	// Установить новый взгляд камеры.
	View.x = Position.x + _newView.x;
	View.y = Position.y + _newView.y;
	View.z = Position.z + _newView.z;
}
void Camera::SetViewByMouse(GLint width, GLint height) {
	POINT mousePos; // Позиция мыши.

	// Вычислить координаты центра окна.
	int middleX = width / 2.0f;
	int middleY = height / 2.0f;

	float angleY = 0.0f; // Направление взгляда вверх/вниз.
	float angleZ = 0.0f; // Значение, необходимое для вращения влево-вправо (по оси Y).
	static float currentRotX = 0.0f;

	// Получить текущие координаты мыши.
	GetCursorPos(&mousePos);

	/*	Если положение мыши не изменилось
		камеру вращать не нужно */
	if (mousePos.x == middleX && mousePos.y == middleY)
		return;

	// Вернуть координаты курсора в центр окна.
	SetCursorPos(middleX, middleY);

	// Определить, куда был сдвинут курсор.
	angleY = (float)((middleX - mousePos.x)) / 1000.0f;
	angleZ = (float)((middleY - mousePos.y)) / 1000.0f;

	/*	Сохраняем последний угол вращения
		и используем заново currentRotX */
	static float lastRotX = 0.0f;
	lastRotX = currentRotX; 

	/*	Если поворот больше одного градуса, умешьшим его
		чтобы уменьшить скорость вращения. */
	if (currentRotX > 1.0f) {
		currentRotX = 1.0f;
		if (lastRotX != 1.0f) {
			/*	Чтобы найти ось, вокруг которой нужно совершать вращение вверх и вниз, нужно
				найти вектор, перпендикулярный вектору взгляда камеры и
				вертикальному вектору */
			Vector3f vAxis = normal(View - Position, UpVector);

			// Нормализуем ось.
			vAxis = normalize(vAxis);

			// Вращаем камеру вокруг нашей оси на заданный угол.
			RotateView(1.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
		}
	}

	// Если угол меньше -1.0f.
	else if (currentRotX < -1.0f) {
		currentRotX = -1.0f;
		if (lastRotX != -1.0f) {
			// Вычисляем ось.
			Vector3f vAxis = normal(View - Position, UpVector);

			// Нормализуем ось.
			vAxis = normalize(vAxis);

			// Вращаем.
			RotateView(-1.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
		}
	} else { // Если в пределах 1.0f -1.0f - просто вращаем.
		Vector3f vAxis = normal(View - Position, UpVector);
		vAxis = normalize(vAxis);
		RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
	}

	// Всегда вращаем камеру вокруг Y-оси.
	RotateView(angleY, 0, 1, 0);
}
void Camera::RotateAroundPoint(Vector3f _Center, float angle, float x, float y, float z) {
	Vector3f _NewPosition;

	// Получим центр, вокруг которого нужно вращатся.
	Vector3f vPos = Position - _Center;

	// Вычислим синус и косинус угла.
	float cosA = (float)cos(angle);
	float sinA = (float)sin(angle);

	// Найдем значение X точки вращения.
	_NewPosition.x = (cosA + (1 - cosA) * x * x) * vPos.x;
	_NewPosition.x += ((1 - cosA) * x * y - z * sinA) * vPos.y;
	_NewPosition.x += ((1 - cosA) * x * z + y * sinA) * vPos.z;

	// Значение Y.
	_NewPosition.y = ((1 - cosA) * x * y + z * sinA) * vPos.x;
	_NewPosition.y += (cosA + (1 - cosA) * y * y) * vPos.y;
	_NewPosition.y += ((1 - cosA) * y * z - x * sinA) * vPos.z;

	// Значение Z.
	_NewPosition.z = ((1 - cosA) * x * z - y * sinA) * vPos.x;
	_NewPosition.z += ((1 - cosA) * y * z + x * sinA) * vPos.y;
	_NewPosition.z += (cosA + (1 - cosA) * z * z) * vPos.z;

	// Установить новую позицию камеры.
	Position = _Center + _NewPosition;
}