#define _USE_MATH_DEFINES 
#include <cmath>
#include <windows.h>
#include "glut.h"
#include <vector>
#include <gl\gl.h>
#include <cstdio>
#include "tracer.h"

#define kSpeed  0.03f	//�������� ����������� ������

using namespace std;

GLint width, height;	//������ � ������ ����
GLfloat ratio;			//����������� ������ � ������ ����
Camera camera;			//������
Scene scene;			//�����
Raytracer raytracer;	//������������

bool rot = false;			//�������� �������� ������ � ������� ����

bool raytracer_mode = false;	//����� �����������
