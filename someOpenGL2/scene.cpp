#include "scene.h"

/*
���������� ���������� ������ "����"
*/
Color::Color() {
	red = green = blue = 0;
}
Color::Color(GLfloat _red, GLfloat _green, GLfloat _blue) {
	red = _red;
	green = _green;
	blue = _blue;
}
void Color::add(GLfloat _red, GLfloat _green, GLfloat _blue) {
	red += _red;
	green += _green;
	blue += _blue;
}
void Color::add(Color colr, Color refl) {
	red += colr.red * refl.red;
	green += colr.green * refl.green;
	blue += colr.blue * refl.blue;
}
void Color::add(Color colr) {
	red += colr.red;
	green += colr.green;
	blue += colr.blue;
}

/*
���������� ���������� ������ "����"
*/
Light::Light(){
}
void Light::setPosition(Vector3f pos){
	position = Vector3f(
		pos.x, 
		pos.y, 
		pos.z
	);
}
void Light::setColor(Color col){
	color = Color(
		col.red, 
		col.green, 
		col.blue
	);
}

/*
���������� ���������� ������ "�����"
*/
Sphere::Sphere() {
}
Sphere::Sphere(GLfloat _radius, Vector3f _center, Color _color) {
	radius = _radius;
	center_coord = _center;
	color = _color;
	FrameColor = Color(255, 0, 0);

	display = true;

	// ��������� �������� ���������
	Ambient[0] = 0.2f;
	Ambient[1] = 0.2f;
	Ambient[2] = 0.2f;
	Ambient[3] = 1.0f;
	Diffuse[0] = 0.8f;
	Diffuse[1] = 0.8f;
	Diffuse[2] = 0.8f;
	Diffuse[3] = 1.0f;
	Specular[0] = 0.0f;
	Specular[1] = 0.0f;
	Specular[2] = 0.0f;
	Specular[3] = 1.0f;
	Emission[0] = 0.0f;
	Emission[1] = 0.0f;
	Emission[2] = 0.0f;
	Emission[3] = 1.0f;
	Shininess = 0.0f;
}
void Sphere::Draw() {
	GLUquadricObj *quadObj;

	// ������� ����� ������ ��� �������� �����.
	quadObj = gluNewQuadric();

	// ��������� ������� �������.
	glPushMatrix();

	// ������������ � ����� ������.
	glTranslated(center_coord.x, center_coord.y, center_coord.z);

	glColor3f(color.red, color.green, color.blue);

	// ������������� �������� ����� �������.
	gluQuadricDrawStyle(quadObj, GLU_FILL);

	// ������ �����.
	gluSphere(quadObj, radius, 10, 10);

	// ��������������� �������.
	glPopMatrix();

	// ������� ������.
	gluDeleteQuadric(quadObj);
}
void Sphere::DrawFrame() {
	GLUquadricObj *quadObj;

	// ������� ����� ������ ��� �������� �����.
	quadObj = gluNewQuadric();

	// ��������� ������� �������.
	glPushMatrix();

	// ������������ � ����� ������.
	glTranslated(center_coord.x, center_coord.y, center_coord.z);

	glColor3f(FrameColor.red, FrameColor.green, FrameColor.blue);

	// ������������� �������� ����� �������.
	gluQuadricDrawStyle(quadObj, GLU_LINE);

	// ������ �����.
	gluSphere(quadObj, radius * 1.01, 10, 10);

	// ��������������� �������.
	glPopMatrix();

	// ������� ������.
	gluDeleteQuadric(quadObj);
}
bool Sphere::hit(Ray ray, Intersection &inter) {
	Ray genRay;

	// ������� ������� ���.
	genRay = transformRay(ray, center_coord, radius, radius, radius);

	double A, B, C;
	A = scal(genRay.dir, genRay.dir);
	B = scal(genRay.start, genRay.dir);
	C = scal(genRay.start, genRay.start) - 1.0;

	// ��������� ������������.
	double discrim = B * B - A * C;

	// ��� ���������� � ��������.
	if (discrim < 0)
		return false;

	int num = 0; // ����� ���������� �� ������ ������.

	double discRoot = sqrt(discrim);
	double t1 = (-B - discRoot) / A; // ����� ������ ����������.

	// ���� ���������� ��������� ������� �����.
	if (t1 > 0.00001) {
		/*	��������� ������ � ����������
			����� ������� ����� ���������� � ���������� ������. */
		inter.hit[0].hitTime = t1;
		inter.hit[0].objectType = 0;
		inter.hit[0].surface = 0;
		inter.hit[0].isEntering = true;

		// ������� ���������� ����� ����������.
		Vector3f P(rayPos(ray, t1));
		inter.hit[0].hitPoint = Vector3f(
			P.x, 
			P.y, 
			P.z
		);

		// ��������� ������� � ����� ����������.
		P = Vector3f(rayPos(genRay, t1));
		inter.hit[0].hitNormal = Vector3f(
			P.x, 
			P.y, 
			P.z
		);
		num = 1;
	}

	double t2 = (-B + discRoot) / A; // ����� ������� ����������.
	if (t2 > 0.00001) {
		/*	��������� ������ � ����������
			����� ������� ����� ���������� � ���������� ������. */
		inter.hit[num].hitTime = t2;
		inter.hit[num].objectType = 0;
		inter.hit[num].surface = 0;
		inter.hit[num].isEntering = false;

		// ������� ���������� ����� ����������.
		Vector3f P(rayPos(ray, t2));
		inter.hit[num].hitPoint = Vector3f(
			P.x, 
			P.y, 
			P.z
		);

		// ��������� ������� � ����� ����������.
		P = Vector3f(rayPos(genRay, t2));
		inter.hit[num].hitNormal = Vector3f(
			P.x, 
			P.y, 
			P.z
		);
		num++;
	}

	inter.numHits = num;
	return (num > 0);
}
bool Sphere::hit(Ray ray) {
	Ray genRay;

	// ������� ������� ���.
	genRay = transformRay(ray, center_coord, radius, radius, radius);

	double A, B, C;
	A = scal(genRay.dir, genRay.dir);
	B = scal(genRay.start, genRay.dir);
	C = scal(genRay.start, genRay.start) - 1.0;

	// ��������� ������������.
	double discrim = B * B - A * C;

	// ��� ���������� � ��������.
	if (discrim < 0)
		return false;

	double discRoot = sqrt(discrim);

	double t1 = (-B - discRoot) / A; // ����� ������ ����������.
	if (t1 >= 0.00001 && t1 <= 1.0)
		return true;

	double t2 = (-B + discRoot) / A; // ����� ������� ����������.
	if (t2 >= 0.00001 && t2 <= 1.0)
		return true;

	return false;
}

/*
���������� ���������� ������ "��������"
*/
Tetrahedron::Tetrahedron() {
}
Tetrahedron::Tetrahedron(Vector3f _coord[], Vector3f _center, Color _color) {
	coord[0] = Vector3f(_coord[0].x, _coord[0].y, _coord[0].z);
	coord[1] = Vector3f(_coord[1].x, _coord[1].y, _coord[1].z);
	coord[2] = Vector3f(_coord[2].x, _coord[2].y, _coord[2].z);
	coord[3] = Vector3f(_coord[3].x, _coord[3].y, _coord[3].z);
	center_coord = _center;
	color = _color;
	FrameColor = Color(255, 0, 0);

	display = true;

	// �������� ��������� �� ���������.
	Ambient[0] = 0.2f;
	Ambient[1] = 0.2f;
	Ambient[2] = 0.2f;
	Ambient[3] = 1.0f;
	Diffuse[0] = 0.8f;
	Diffuse[1] = 0.8f;
	Diffuse[2] = 0.8f;
	Diffuse[3] = 1.0f;
	Specular[0] = 0.0f;
	Specular[1] = 0.0f;
	Specular[2] = 0.0f;
	Specular[3] = 1.0f;
	Emission[0] = 0.0f;
	Emission[1] = 0.0f;
	Emission[2] = 0.0f;
	Emission[3] = 1.0f;
	Shininess = 0.0f;
}
void Tetrahedron::Draw() {
	// ��������� ������� �������.
	glPushMatrix();

	// ������������ � ����� ������.
	glTranslated(center_coord.x, center_coord.y, center_coord.z);

	// ������ ��������.
	glBegin(GL_POLYGON);
	glColor3f(color.red, color.green, color.blue);
	glVertex3f(coord[0].x, coord[0].y, coord[0].z);
	glVertex3f(coord[1].x, coord[1].y, coord[1].z);
	glVertex3f(coord[2].x, coord[2].y, coord[2].z);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(color.red, color.green, color.blue);
	glVertex3f(coord[1].x, coord[1].y, coord[1].z);
	glVertex3f(coord[3].x, coord[3].y, coord[3].z);
	glVertex3f(coord[2].x, coord[2].y, coord[2].z);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(color.red, color.green, color.blue);
	glVertex3f(coord[3].x, coord[3].y, coord[3].z);
	glVertex3f(coord[0].x, coord[0].y, coord[0].z);
	glVertex3f(coord[2].x, coord[2].y, coord[2].z);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(color.red, color.green, color.blue);
	glVertex3f(coord[1].x, coord[1].y, coord[1].z);
	glVertex3f(coord[0].x, coord[0].y, coord[0].z);
	glVertex3f(coord[3].x, coord[3].y, coord[3].z);
	glEnd();

	// ��������������� �������.
	glPopMatrix();
}
void Tetrahedron::DrawFrame() {
	// ��������� ������� �������.
	glPushMatrix();

	// ������������ � ����� ������.
	glTranslated(center_coord.x, center_coord.y, center_coord.z);

	// ������ ��������.
	glBegin(GL_LINE_LOOP);
	glLineWidth(5);
	glColor3f(FrameColor.red, FrameColor.green, FrameColor.blue);
	glVertex3f(coord[0].x, coord[0].y, coord[0].z);
	glVertex3f(coord[1].x, coord[1].y, coord[1].z);
	glVertex3f(coord[2].x, coord[2].y, coord[2].z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3f(FrameColor.red, FrameColor.green, FrameColor.blue);
	glVertex3f(coord[1].x, coord[1].y, coord[1].z);
	glVertex3f(coord[3].x, coord[3].y, coord[3].z);
	glVertex3f(coord[2].x, coord[2].y, coord[2].z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3f(FrameColor.red, FrameColor.green, FrameColor.blue);
	glVertex3f(coord[3].x, coord[3].y, coord[3].z);
	glVertex3f(coord[0].x, coord[0].y, coord[0].z);
	glVertex3f(coord[2].x, coord[2].y, coord[2].z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3f(FrameColor.red, FrameColor.green, FrameColor.blue);
	glVertex3f(coord[1].x, coord[1].y, coord[1].z);
	glVertex3f(coord[0].x, coord[0].y, coord[0].z);
	glVertex3f(coord[3].x, coord[3].y, coord[3].z);
	glEnd();

	// ��������������� �������.
	glPopMatrix();
}
bool Tetrahedron::hit(Ray ray, Intersection &inter) {
	float timeIn[4]; // ����� ���������� � ������ ����������.
	Vector3f N[4];   // ������� � ������ ���������.

	// ����� ����������� ����� 0.
	inter.numHits = 0;

	// ��� � ������� �����������.
	Ray genRay = transformRay(ray, center_coord, 1, 1, 1);

	// ���� ��� ����������� � ������ ���������� (0 1 2).
	if (!hitPlane(genRay, 0, 1, 2, N[0], timeIn[0]))
		timeIn[0] = -10000.0;
	else
		inter.numHits++;

	// ���� ��� ����������� �� ������ ���������� (1 3 2).
	if (!hitPlane(genRay, 2, 3, 1, N[1], timeIn[1]))
		timeIn[1] = -10000.0;
	else
		inter.numHits++;

	// ���� ��� ����������� � ������� ���������� (3 0 2).
	if (!hitPlane(genRay, 3, 0, 2, N[2], timeIn[2]))
		timeIn[2] = -10000.0;
	else
		inter.numHits++;

	// ���� ��� ����������� � ��������� ���������� (1 0 3).
	if (!hitPlane(genRay, 3, 0, 1, N[3], timeIn[3]))
		timeIn[3] = -10000.0;
	else
		inter.numHits++;

	/*	������, ��� ����� ������(������ ����) ��������� � ��������� �� ���������������
		���� ���������� ������ ����, ���������� ���. */
	if (inter.numHits < 2)
		return false;
	else {
		// ���������� ����� ����� � ����� ������ �� ������.
		int max;
		int min;

		max = 0;

		for (int i = 1; i < 4; i++)
			if (timeIn[i] > timeIn[max])
				max = i;

		min = max;
		for (int i = 0; i < 4; i++) {
			if (timeIn[i] < timeIn[min] && timeIn[i] > 0.0)
				min = i;
		}

		// ���� ������������ ����� ������������ - ���������� ���.
		if (timeIn[max] < 0.0)
			return false;

		// ��������� ������ � �����������.
		inter.hit[0].objectType = 1;
		inter.hit[0].hitTime = timeIn[min];
		inter.hit[0].isEntering = true;
		inter.hit[0].surface = min;

		// ������� ���������� ����� ����������.
		Vector3f P(rayPos(ray, timeIn[min]));
		inter.hit[0].hitPoint = Vector3f(P.x, P.y, P.z);

		// ������� � ����� ����������.
		inter.hit[0].hitNormal = N[min];

		inter.hit[1].objectType = 1;
		inter.hit[1].hitTime = timeIn[max];
		inter.hit[1].isEntering = false;
		inter.hit[1].surface = max;

		// ������� ���������� ����� ����������.
		Vector3f C(rayPos(ray, timeIn[max]));
		inter.hit[1].hitPoint = Vector3f(C.x, C.y, C.z);

		// ������� � ����� ����������.
		inter.hit[1].hitNormal = N[max];

		return true;
	}
}
bool Tetrahedron::hit(Ray ray) {
	float timeIn;  // ����� ���������� � ������ ����������.
	Vector3f N[4]; // ������� � ������ ���������.

	// ����� ����������� ����� 0.
	int num = 0;

	// ��� � ������� �����������.
	Ray genRay = transformRay(ray, center_coord, 1, 1, 1);

	// ���� ����������� � ������ ���������� (0 1 2).
	if (hitPlane(genRay, 0, 1, 2, N[0], timeIn))
		if (timeIn >= 0.0 && timeIn <= 1.0)
			return true;

	// ���� ����������� �� ������ ���������� (2 3 1).
	if (hitPlane(genRay, 2, 3, 1, N[1], timeIn))
		if (timeIn >= 0.0 && timeIn <= 1.0)
			return true;

	// ���� ����������� � ������� ���������� (3 0 2).
	if (hitPlane(genRay, 3, 0, 2, N[2], timeIn))
		if (timeIn >= 0.0 && timeIn <= 1.0)
			return true;

	// ����  ����������� � ��������� ���������� (3 0 1).
	if (hitPlane(genRay, 3, 0, 1, N[3], timeIn))
		if (timeIn >= 0.0 && timeIn <= 1.0)
			return true;

	return false;
}
bool Tetrahedron::hitPlane(Ray ray, int ver1, int ver2, int ver3, Vector3f &_N, float &time) {
	Vector3f P0;		// ��������� ����� ����.
	Vector3f Dir;		// ����������� ����.
	Vector3f A, B, C;	// ������� ������������.
	Vector3f N;			// ������� ������������.

	Vector3f O;			// ����� �����������.

	P0 = ray.start;
	Dir = ray.dir;
	A = coord[ver1];
	B = coord[ver2];
	C = coord[ver3];
	N = count_normal_for_polygon(A, B, C);

	float vn = scalar(Dir, N);

	// ������� ����� ����������.
	Vector3f V = P0 - A;
	float t = -scalar(V, N) / vn;
	V = Dir * t;
	O = P0 + V;

	// ���������� �������������� ������������.
	// �������� ��������� ��� �������� YZ.
	if (fabs(N.x) >= fabs(N.y) && fabs(N.x) >= fabs(N.z)) {
		if ((O.z - A.z) * (B.y - A.y) - (O.y - A.y) * (B.z - A.z) < 0)
			return false;
		if ((O.z - B.z) * (C.y - B.y) - (O.y - B.y) * (C.z - B.z) < 0)
			return false;
		if ((O.z - C.z) * (A.y - C.y) - (O.y - C.y) * (A.z - C.z) < 0)
			return false;
	} else if (fabs(N.z) >= fabs(N.y)) { // �������� ��������� ��� �������� XY.
		if ((O.y - A.y) * (B.x - A.x) - (O.x - A.x) * (B.y - A.y) < 0)
			return false;
		if ((O.y - B.y) * (C.x - B.x) - (O.x - B.x) * (C.y - B.y) < 0)
			return false;
		if ((O.y - C.y) * (A.x - C.x) - (O.x - C.x) * (A.y - C.y) < 0)
			return false;
	} else { // �������� ��������� ��� �������� XZ.
		if ((O.z - A.z) * (B.x - A.x) - (O.x - A.x) * (B.z - A.z) < 0)
			return false;
		if ((O.z - B.z) * (C.x - B.x) - (O.x - B.x) * (C.z - B.z) < 0)
			return false;
		if ((O.z - C.z) * (A.x - C.x) - (O.x - C.x) * (A.z - C.z) < 0)
			return false;
	}
	_N = N;
	time = t;
	return true;
}

/*
���������� ���������� ������ "���"
*/
Square::Square() {
}
Square::Square(Vector3f _center, GLfloat _scaleX, GLfloat _scaleZ, Color _color) {
	center_coord = _center;
	scaleX = _scaleX;
	scaleZ = _scaleZ;
	color = _color;

	// �������� ��������� �� ���������.
	Ambient[0] = 0.2f;
	Ambient[1] = 0.2f;
	Ambient[2] = 0.2f;
	Ambient[3] = 1.0f;
	Diffuse[0] = 0.8f;
	Diffuse[1] = 0.8f;
	Diffuse[2] = 0.8f;
	Diffuse[3] = 1.0f;
	Specular[0] = 0.2f;
	Specular[1] = 0.2f;
	Specular[2] = 0.2f;
	Specular[3] = 1.0f;
	Emission[0] = 0.0f;
	Emission[1] = 0.0f;
	Emission[2] = 0.0f;
	Emission[3] = 1.0f;
	Shininess = 0.6f;
}
void Square::Draw() {
	// ��������� ������� �������.
	glPushMatrix();

	// ������������ �������.
	glScalef(scaleX, 1, scaleZ);

	// ������������ � ����� ������.
	glTranslated(center_coord.x, center_coord.y, center_coord.z);

	// ������ �������.
	glBegin(GL_POLYGON);
	glColor3f(color.red, color.green, color.blue);
	glVertex3f(-1, 0, -1);
	glVertex3f(-1, 0, 1);
	glVertex3f(1, 0, 1);
	glVertex3f(1, 0, -1);
	glEnd();

	// ��������������� �������.
	glPopMatrix();
}
bool Square::hit(Ray ray, Intersection &inter) {
	Ray genRay;

	// ������� � �������� ����.
	genRay = transformRay(ray, center_coord, scaleX, 1, scaleZ);

	inter.numHits = 0;

	double denom = genRay.dir.y;

	// ��� ���������� ���������.
	if (fabs(denom) < 0.0001)
		return false;

	double time = -genRay.start.y / denom; // ����� ����������.

	// ������� ����� ������ �������.
	if (time <= 0.0)
		return false;

	double hx = genRay.start.x + genRay.dir.x * time;
	double hz = genRay.start.z + genRay.dir.z * time;

	// �������� ���� � ����������� �.
	if (hx > 1.0 || hx < -1.0)
		return false;

	// �������� ���� � ����������� y.
	if (hz > 1.0 || hz < -1.0)
		return false;

	// ���� ����������.
	inter.numHits = 1;
	inter.hit[0].objectType = 10;
	inter.hit[0].hitTime = time;
	inter.hit[0].isEntering = true;
	inter.hit[0].surface = 0;

	// ����� ���������� � ������� �����������.
	Vector3f P(rayPos(ray, time));
	inter.hit[0].hitPoint = Vector3f(P.x, P.y, P.z);

	// ������� � �������.
	inter.hit[0].hitNormal = Vector3f(0, 1, 0);

	return true;
}
bool Square::hit(Ray ray) {
	Ray genRay;

	// ������� � �������� ����.
	genRay = transformRay(ray, center_coord, scaleX, 1, scaleZ);

	double denom = genRay.dir.y;

	// ��� ���������� ���������.
	if (fabs(denom) < 0.0001)
		return false;

	double time = -genRay.start.y / denom; // ����� ����������.

										   // ������� ����� ������ �������.
	if (time <= 0.0)
		return false;

	double hx = genRay.start.x + genRay.dir.x * time;
	double hz = genRay.start.z + genRay.dir.z * time;

	// �������� ���� � ����������� �.
	if (hx > 1.0 || hx < -1.0)
		return false;

	// �������� ���� � ����������� y.
	if (hz > 1.0 || hz < -1.0)
		return false;

	if (time >= 0 && time <= 1)
		return true;

	return false;
}

/*
���������� ���������� ������ "�����"
*/
Scene::Scene() {
	set_data();

	add_del = false;
	sphere_mod = false;
	tetrahedron_mod = false;
	active_Sphere = 0;
	active_Tetrahedron = 0;
}
void Scene::set_data() {
	FILE *in, *in_mat;
	Sphere _Sphere;
	Tetrahedron _Tetrahedron;
	int type;

	in = fopen("scene_objects.txt", "r");
	in_mat = fopen("scene_materials.txt", "r");

	while (!feof(in)) {
		fscanf(in, "%d", &type);
		switch (type) {
		case 0: { // �����.
			float _radius;
			Vector3f _center;
			Color _color;

			fscanf(in, "%f", &_radius);
			fscanf(in, "%f%f%f", &_center.x, &_center.y, &_center.z);
			fscanf(in, "%f%f%f", &_color.red, &_color.green, &_color.blue);

			_Sphere = Sphere(_radius, _center, _color);

			int is;

			fscanf(in_mat, "%d", &is);
			// ���� ������� ������� ���������.
			if (is)
				for (int i = 0; i < 4; i++)
					fscanf(in_mat, "%f", &_Sphere.Ambient[i]);

			fscanf(in_mat, "%d", &is);
			// ���� ������� ���������� ���������.
			if (is)
				for (int i = 0; i < 4; i++)
					fscanf(in_mat, "%f", &_Sphere.Diffuse[i]);

			fscanf(in_mat, "%d", &is);
			// ���� ������� ���������� ���������.
			if (is)
				for (int i = 0; i < 4; i++)
					fscanf(in_mat, "%f", &_Sphere.Specular[i]);

			fscanf(in_mat, "%d", &is);
			// ���� ������� ����������� ���������.
			if (is)
				for (int i = 0; i < 4; i++)
					fscanf(in_mat, "%f", &_Sphere.Emission[i]);

			fscanf(in_mat, "%d", &is);
			// ���� ������ ����������� ������.
			if (is)
				fscanf(in_mat, "%f", &_Sphere.Shininess);

			vector_Sphere.push_back(_Sphere);
		}
		break;
		case 1: { // ��������.
			Vector3f _coord[4];
			Vector3f _center;
			Color _color;

			for (int i = 0; i < 4; i++)
				fscanf(in, "%f%f%f", &_coord[i].x, &_coord[i].y, &_coord[i].z);
			fscanf(in, "%f%f%f", &_center.x, &_center.y, &_center.z);
			fscanf(in, "%f%f%f", &_color.red, &_color.green, &_color.blue);

			_Tetrahedron = Tetrahedron(_coord, _center, _color);

			int is;

			fscanf(in_mat, "%d", &is);
			// ���� ������� ������� ���������.
			if (is)
				for (int i = 0; i < 4; i++)
					fscanf(in_mat, "%f", &_Tetrahedron.Ambient[i]);

			fscanf(in_mat, "%d", &is);
			// ���� ������� ���������� ���������.
			if (is)
				for (int i = 0; i < 4; i++)
					fscanf(in_mat, "%f", &_Tetrahedron.Diffuse[i]);

			fscanf(in_mat, "%d", &is);
			// ���� ������� ���������� ���������.
			if (is)
				for (int i = 0; i < 4; i++)
					fscanf(in_mat, "%f", &_Tetrahedron.Specular[i]);

			fscanf(in_mat, "%d", &is);
			// ���� ������� ����������� ���������.
			if (is)
				for (int i = 0; i < 4; i++)
					fscanf(in_mat, "%f", &_Tetrahedron.Emission[i]);

			fscanf(in_mat, "%d", &is);
			// ���� ������ ����������� ������.
			if (is)
				fscanf(in_mat, "%f", &_Tetrahedron.Shininess);

			vector_Tetrahedron.push_back(_Tetrahedron);
		}
		break;
		}
	}
	fclose(in);
	fclose(in_mat);

	// ���������.
	square = Square(Vector3f(0, -2, 0), 10, 10, Color(1, 0.3, 0));

	// ��������� ���������
	Light _Light;
	_Light.position = Vector3f(-5, 2, -5);
	_Light.color = Color(0.4, 0.4, 0.4);

	vector_Light.push_back(_Light);

	_Light.position = Vector3f(-13.0, 1.0, 8.0);
	_Light.color = Color(0.1, 0.2, 0.5);
	vector_Light.push_back(_Light);
}
void Scene::switch_forward() {
	// ������ �� �������.
	if (sphere_mod)	{
		active_Sphere++;
		if (active_Sphere == vector_Sphere.size())
			active_Sphere = 0;
	}

	// ������ � �����������.
	if (tetrahedron_mod) {
		active_Tetrahedron++;
		if (active_Tetrahedron == vector_Tetrahedron.size())
			active_Tetrahedron = 0;
	}
}
void Scene::switch_backward() {
	// ������ �� �������.
	if (sphere_mod) {
		active_Sphere--;
		if (active_Sphere == -1)
			active_Sphere = vector_Sphere.size() - 1;
	}

	// ������ � �����������.
	if (tetrahedron_mod) {
		active_Tetrahedron--;
		if (active_Tetrahedron == -1)
			active_Tetrahedron = vector_Tetrahedron.size() - 1;
	}
}
void Scene::Draw() {
	for (int i = 0; i < vector_Sphere.size(); i++) {
		if (vector_Sphere[i].display)
			vector_Sphere[i].Draw();

		if (add_del && sphere_mod && i == active_Sphere)
			vector_Sphere[i].DrawFrame();
	}

	for (int i = 0; i < vector_Tetrahedron.size(); i++)	{
		if (vector_Tetrahedron[i].display)
			vector_Tetrahedron[i].Draw();

		if (add_del && tetrahedron_mod && i == active_Tetrahedron)
			vector_Tetrahedron[i].DrawFrame();
	}

	square.Draw();
}
bool Scene::isInShadow(Ray ray) {
	for (int i = 0; i < vector_Sphere.size(); i++)
		if (vector_Sphere[i].display)
			if (vector_Sphere[i].hit(ray))
				return true;
	for (int i = 0; i < vector_Tetrahedron.size(); i++)
		if (vector_Tetrahedron[i].display)
			if (vector_Tetrahedron[i].hit(ray))
				return true;
	if (square.hit(ray))
		return true;
	return false;
}

//=============��� �������������� ���� � �������� ����==============
// ����������� ������� �������������.
void makeTransformMatrix(double **TMatrix, Vector3f shift, GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ) {
	TMatrix[0][0] = scaleX;
	TMatrix[0][1] = 0;
	TMatrix[0][2] = 0;
	TMatrix[0][3] = shift.x;
	TMatrix[1][0] = 0;
	TMatrix[1][1] = scaleY;
	TMatrix[1][2] = 0;
	TMatrix[1][3] = shift.y;
	TMatrix[2][0] = 0;
	TMatrix[2][1] = 0;
	TMatrix[2][2] = scaleZ;
	TMatrix[2][3] = shift.z;
	TMatrix[3][0] = 0;
	TMatrix[3][1] = 0;
	TMatrix[3][2] = 0;
	TMatrix[3][3] = 1;
}

// ���������� ���� � �������� ����.
Ray transformRay(Ray _ray, Vector3f shift, GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ) {
	double **TMatrix;
	TMatrix = new double *[4];
	for (int i = 0; i < 4; i++)
		TMatrix[i] = new double[4];

	// ���������� ������� �������������.
	makeTransformMatrix(TMatrix, shift, scaleX, scaleY, scaleZ);

	// ���������� ������ ���� � ����������� � ���������� �����������.
	double point[4] = { _ray.start.x, _ray.start.y, _ray.start.z, 1.0f };
	double dir[4] = { _ray.dir.x, _ray.dir.y, _ray.dir.z, 0.0f };

	double sol_point[4];
	double sol_dir[4];

	// ����� ������� ���������� ������ ���� � ��� �����������.
	solve_gauss(TMatrix, point, sol_point, 4);
	solve_gauss(TMatrix, dir, sol_dir, 4);

	// ��������� � ������� ����������.
	Vector3f newPoint = Vector3f(sol_point[0], sol_point[1], sol_point[2]);
	Vector3f newDir = Vector3f(sol_dir[0], sol_dir[1], sol_dir[2]);

	// ������� ����� ���.
	Ray newRay;
	newRay.SetStart(newPoint);
	newRay.SetDir(newDir);

	for (int i = 0; i < 4; i++)
		delete[] TMatrix[i];
	delete[] TMatrix;

	// ������� ����� ���.
	return newRay;
}

// ��������� ������������.
double scal(Vector3f vec1, Vector3f vec2) {
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}

// ���������� ������� ����.
Vector3f rayPos(Ray ray, double t) {
	Vector3f vec;
	vec = Vector3f(ray.start.x + ray.dir.x * t,
		ray.start.y + ray.dir.y * t,
		ray.start.z + ray.dir.z * t);
	return vec;
}