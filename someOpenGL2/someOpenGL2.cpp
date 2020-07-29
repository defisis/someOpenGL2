#define _CRT_SECURE_NO_WARNINGS
#include "Hook.h"

void reshape(GLint w, GLint h)
{
	if (!raytracer_mode)
	{
		//изменяем размеры окна
		width = w;
		height = h;

		/* вычисляем соотношение между шириной и высотой */

		//предотвращаем деление на 0
		if (height == 0)
			height = 1;
		ratio = 1. * width / height;

		//устанавливаем матрицу проекции (определяет объем сцены)
		glMatrixMode(GL_PROJECTION);

		//загружаем единичную матрицу
		glLoadIdentity();

		//определяем окно просмотра
		glViewport(0, 0, width, height);

		//перспективная проекция
		gluPerspective(60, ratio, 0.1f, 100.0f);

		//возвращаемся к матрице модели
		glMatrixMode(GL_MODELVIEW);
	}
	else
	{
		//меняем размеры окна
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

//отрисовка сетки
/*void draw_grid()
{
	glColor3ub(0, 255, 0);
	for (float i = -50; i <= 50; i += 1)
	{
		glBegin(GL_LINES);

		//ось Х
		glVertex3f(-50, 0, i);
		glVertex3f(50, 0, i);

		//ось Z
		glVertex3f(i, 0, -50);
		glVertex3f(i, 0, 50);
		glEnd();
	}
}*/

//отрисовка на экран
void display(void)
{
	//очистка буфера цвета и глубины
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//включаем буфер глубины
	glEnable(GL_DEPTH_TEST);

	//обнуляем трансформации (т.е. загружаем единичную матрицу)
	glLoadIdentity();

	//устанавливаем вид камеры
	gluLookAt(camera.Position.x, camera.Position.y, camera.Position.z,
		camera.View.x, camera.View.y, camera.View.z,
		camera.UpVector.x, camera.UpVector.y, camera.UpVector.z);

	//отрисовка сетки
	//draw_grid();

	//отрисовка сцены
	scene.Draw();

	//если включен режим трассировки
	if (raytracer_mode)
	{
		//устанавливаем параметры трассировщика
		raytracer = Raytracer(camera.Position, scene, camera, ratio, width, height, 1);

		//запустить трассировку
		raytracer.Raytrace();
	}

	glutPostRedisplay();

	//сменить буфер
	glutSwapBuffers();
}

//реакция на движение мыши
void mouse_move(int x, int y)
{
	if (rot)
		camera.SetViewByMouse(width, height);
}

//реакция на нажатие клавиш клавиатуры
void press_key(unsigned char key, int x, int y)
{
	//включение/выключение вращения камеры мышью
	if (key == 'q' || key == 'Q')
	{
		rot = !rot;
		ShowCursor(!rot);
	}

	//включение/выключение трассировки
	if (key == 't' || key == 'T')
	{
		raytracer_mode = !raytracer_mode;
		if (!raytracer_mode)
			reshape(width, height);
	}

	//включение/выключение режима включения/исключения фигур
	if (key == 'c' || key == 'C')
	{
		//переключить режим
		scene.add_del = !scene.add_del;

		//если режим включен
		if (scene.add_del)
		{
			//задать начальные установки
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

	//включить/исключить фигуру
	if (key == 'e' || key == 'E')
	{
		//если включен режим включения/исключения фигур
		if (scene.add_del)
		{
			if (scene.sphere_mod)
				scene.vector_Sphere[scene.active_Sphere].display = !scene.vector_Sphere[scene.active_Sphere].display;

			if (scene.tetrahedron_mod)
				scene.vector_Tetrahedron[scene.active_Tetrahedron].display = !scene.vector_Tetrahedron[scene.active_Tetrahedron].display;
		}
	}

	//движение камеры
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

	//перемещение источника света
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
	//переключать фигуры вперед
	if (key == GLUT_KEY_UP)
	{
		if (scene.add_del)
			scene.switch_forward();
	}

	//переключать фигуры назад
	if (key == GLUT_KEY_DOWN)
	{
		if (scene.add_del)
			scene.switch_backward();
	}

	//сменить активный контейнер фигур
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

//иницализация
void init()
{
	ratio = 1. * width / height;
	camera.PositionCamera(-12.0f, 4.0f, -3.0f, -5.5f, 1.0f, -1.5f, 0.0f, 1.0f, 0.0f);	//установка начальной позиции камеры
}

int main(int argc, char* argv[])
{
	width = 500;
	height = 500;
	glutInit(&argc, argv);

	//включить буфер глубины/двойную буферизацию
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutCreateWindow("KG_4");
	init();								//начальные установки

	glutKeyboardFunc(press_key);		//обработка клавиш с кодами ascii
	glutSpecialFunc(press_special_key);		//обработка не-ascii клавиш

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutPassiveMotionFunc(mouse_move);

	glutMainLoop();
}