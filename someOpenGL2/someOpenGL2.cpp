#define _CRT_SECURE_NO_WARNINGS
#include "Hook.h"

void reshape(GLint w, GLint h)
{
	if (!raytracer_mode)
	{
		//�������� ������� ����
		width = w;
		height = h;

		/* ��������� ����������� ����� ������� � ������� */

		//������������� ������� �� 0
		if (height == 0)
			height = 1;
		ratio = 1. * width / height;

		//������������� ������� �������� (���������� ����� �����)
		glMatrixMode(GL_PROJECTION);

		//��������� ��������� �������
		glLoadIdentity();

		//���������� ���� ���������
		glViewport(0, 0, width, height);

		//������������� ��������
		gluPerspective(60, ratio, 0.1f, 100.0f);

		//������������ � ������� ������
		glMatrixMode(GL_MODELVIEW);
	}
	else
	{
		//������ ������� ����
		width = w;
		height = h;

		if (height == 0)
			height = 1;
		ratio = 1. * width / height;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(0, 0, width, height);
		gluOrtho2D(0.0, width, 0.0, height);
	}
}

//��������� �����
/*void draw_grid()
{
	glColor3ub(0, 255, 0);
	for (float i = -50; i <= 50; i += 1)
	{
		glBegin(GL_LINES);

		//��� �
		glVertex3f(-50, 0, i);
		glVertex3f(50, 0, i);

		//��� Z
		glVertex3f(i, 0, -50);
		glVertex3f(i, 0, 50);
		glEnd();
	}
}*/

//��������� �� �����
void display(void)
{
	//������� ������ ����� � �������
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//�������� ����� �������
	glEnable(GL_DEPTH_TEST);

	//�������� ������������� (�.�. ��������� ��������� �������)
	glLoadIdentity();

	//������������� ��� ������
	gluLookAt(camera.Position.x, camera.Position.y, camera.Position.z,
		camera.View.x, camera.View.y, camera.View.z,
		camera.UpVector.x, camera.UpVector.y, camera.UpVector.z);

	//��������� �����
	//draw_grid();

	//��������� �����
	scene.Draw();

	//���� ������� ����� �����������
	if (raytracer_mode)
	{
		//������������� ��������� �������������
		raytracer = Raytracer(camera.Position, scene, camera, ratio, width, height, 1);

		//��������� �����������
		raytracer.Raytrace();
	}

	glutPostRedisplay();

	//������� �����
	glutSwapBuffers();
}

//������� �� �������� ����
void mouse_move(int x, int y)
{
	if (rot)
		camera.SetViewByMouse(width, height);
}

//������� �� ������� ������ ����������
void press_key(unsigned char key, int x, int y)
{
	//���������/���������� �������� ������ �����
	if (key == 'q' || key == 'Q')
	{
		rot = !rot;
		ShowCursor(!rot);
	}

	//���������/���������� �����������
	if (key == 't' || key == 'T')
	{
		raytracer_mode = !raytracer_mode;
		if (!raytracer_mode)
			reshape(width, height);
	}

	//���������/���������� ������ ���������/���������� �����
	if (key == 'c' || key == 'C')
	{
		//����������� �����
		scene.add_del = !scene.add_del;

		//���� ����� �������
		if (scene.add_del)
		{
			//������ ��������� ���������
			scene.sphere_mod = true;
			scene.tetrahedron_mod = false;
			scene.active_Sphere = 0;
			scene.active_Tetrahedron = 0;
		}

		else
		{
			scene.sphere_mod = false;
			scene.tetrahedron_mod = false;
		}
	}

	//��������/��������� ������
	if (key == 'e' || key == 'E')
	{
		//���� ������� ����� ���������/���������� �����
		if (scene.add_del)
		{
			if (scene.sphere_mod)
				scene.vector_Sphere[scene.active_Sphere].display = !scene.vector_Sphere[scene.active_Sphere].display;

			if (scene.tetrahedron_mod)
				scene.vector_Tetrahedron[scene.active_Tetrahedron].display = !scene.vector_Tetrahedron[scene.active_Tetrahedron].display;
		}
	}

	//�������� ������
	if (key == 'w' || key == 'W')
	{
		camera.MoveCamera(kSpeed);
	}
	if (key == 's' || key == 'S')
	{
		camera.MoveCamera(-kSpeed);
	}
	if (key == 'a' || key == 'A')
	{
		camera.RotateAroundPoint(camera.View, -kSpeed * 2.0f, 0.0f, 1.0f, 0.0f);
	}
	if (key == 'd' || key == 'D')
	{
		camera.RotateAroundPoint(camera.View, kSpeed * 2.0f, 0.0f, 1.0f, 0.0f);
	}

	//����������� ��������� �����
	if (key == '8')
	{
		scene.vector_Light[0].position.y++;
		printf("%0.0f %0.0f %0.0f\n", scene.vector_Light[0].position.x, scene.vector_Light[0].position.y, scene.vector_Light[0].position.z);
	}
	if (key == '5')
	{
		scene.vector_Light[0].position.y--;
		printf("%0.0f %0.0f %0.0f\n", scene.vector_Light[0].position.x, scene.vector_Light[0].position.y, scene.vector_Light[0].position.z);
	}
	if (key == '7')
	{
		scene.vector_Light[0].position.x++;
		printf("%0.0f %0.0f %0.0f\n", scene.vector_Light[0].position.x, scene.vector_Light[0].position.y, scene.vector_Light[0].position.z);
	}
	if (key == '4')
	{
		scene.vector_Light[0].position.x--;
		printf("%0.0f %0.0f %0.0f\n", scene.vector_Light[0].position.x, scene.vector_Light[0].position.y, scene.vector_Light[0].position.z);
	}
	if (key == '9')
	{
		scene.vector_Light[0].position.z++;
		printf("%0.0f %0.0f %0.0f\n", scene.vector_Light[0].position.x, scene.vector_Light[0].position.y, scene.vector_Light[0].position.z);
	}
	if (key == '6')
	{
		scene.vector_Light[0].position.z--;
		printf("%0.0f %0.0f %0.0f\n", scene.vector_Light[0].position.x, scene.vector_Light[0].position.y, scene.vector_Light[0].position.z);
	}

}

void press_special_key(int key, int x, int y)
{
	//����������� ������ ������
	if (key == GLUT_KEY_UP)
	{
		if (scene.add_del)
			scene.switch_forward();
	}

	//����������� ������ �����
	if (key == GLUT_KEY_DOWN)
	{
		if (scene.add_del)
			scene.switch_backward();
	}

	//������� �������� ��������� �����
	if (key == GLUT_KEY_LEFT)
	{
		if (scene.add_del)
			if (scene.sphere_mod)
			{
				if (scene.vector_Tetrahedron.size() > 0)
				{
					scene.sphere_mod = false;
					scene.tetrahedron_mod = true;
				}
			}
			else
			{
				if (scene.vector_Sphere.size() > 0)
				{
					scene.sphere_mod = true;
					scene.tetrahedron_mod = false;
				}
			}
	}
	glutPostRedisplay();
}

//������������
void init()
{
	ratio = 1. * width / height;
	camera.PositionCamera(-12.0f, 4.0f, -3.0f, -5.5f, 1.0f, -1.5f, 0.0f, 1.0f, 0.0f);	//��������� ��������� ������� ������
}

int main(int argc, char* argv[])
{
	width = 500;
	height = 500;
	glutInit(&argc, argv);

	//�������� ����� �������/������� �����������
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutCreateWindow("KG_4");
	init();								//��������� ���������

	glutKeyboardFunc(press_key);		//��������� ������ � ������ ascii
	glutSpecialFunc(press_special_key);		//��������� ��-ascii ������

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutPassiveMotionFunc(mouse_move);

	glutMainLoop();
}