#include "normals.h"

//вычисляем нормаль двух векторов
Vector3f normal(Vector3f vect_1, Vector3f vect_2)
{
	Vector3f norm;

	//вычисление векторного произведения
	norm.x = ((vect_1.y*vect_2.z) - (vect_1.z*vect_2.y));
	norm.y = ((vect_1.z*vect_2.x) - (vect_1.x*vect_2.z));
	norm.z = ((vect_1.x*vect_2.y) - (vect_1.y*vect_2.x));

	return norm;
}

//вычисляем норму вектора
float norma(Vector3f vect)
{
	return (float)sqrt((vect.x*vect.x) + (vect.y*vect.y) + (vect.z*vect.z));
}

//нормализуем вектор
Vector3f normalize(Vector3f vect)
{
	//Вычислить норму вектора
	float norm = norma(vect);

	//нормализовать вектор
	vect = vect / norm;

	return vect;
}

//вычисляем скалярное произведение
float scalar(Vector3f vect_1, Vector3f vect_2)
{
	return vect_1.x*vect_2.x + vect_1.y*vect_2.y + vect_1.z*vect_2.z;
}

//вычисляем вектор между двумя точками
Vector3f count_vect(Vector3f point_1, Vector3f point_2)
{
	Vector3f vect;

	vect.x = point_1.x - point_2.x;
	vect.y = point_1.y - point_2.y;
	vect.z = point_1.z - point_2.z;
	return vect;
}

//вычисляем нормали полигона
Vector3f count_normal_for_polygon(Vector3f one, Vector3f two, Vector3f three)
{
	Vector3f vect_1 = count_vect(three, two);
	Vector3f vect_2 = count_vect(two, one);
	Vector3f norm = normal(vect_1, vect_2);
	norm = normalize(norm);
	return norm;
}