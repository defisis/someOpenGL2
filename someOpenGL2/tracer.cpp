#include"tracer.h"

/*
���������� ���������� ������ ���������� � ����� "tracer.h".
*/

Raytracer::Raytracer() {

}

Raytracer::Raytracer(
	Vector3f _eye, 
	Scene _scene, 
	Camera _camera, 
	float _aspect, 
	int _nCols, 
	int _nRows, 
	int _blockSize
) {
	eye = _eye;
	scene = _scene;
	camera = _camera;
	aspect = _aspect;
	nCols = _nCols;
	nRows = _nRows;
	blockSize = _blockSize;
}

void Raytracer::Raytrace() {
	Ray ray;		// ���.
	Color color;	// ����.
	float tetha = 60 * (M_PI / 180.);	// ���� ������.
	float N = 0.1;						// ���������� �� ����� �������, �� ������� ���������.
	Vector3f n = camera.Position - camera.View;
	Vector3f u = normal(camera.UpVector, n);
	Vector3f v = normal(n, u);

	// ������������ �.�., ��������� � �������.
	n = normalize(n);
	u = normalize(u);
	v = normalize(v);

	float H = N * tan(tetha / 2.0);
	float W = H * aspect;

	// ���������� ���������� ������ ����.
	ray.SetStart(eye);

	// ������� ������� ��������.
	ray.recurseLevel = 0;
	nCols = 250;
	nRows = 250;
	// ��������� OpenGl ��� 2D-���������.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, nCols, 0, nRows);
	glDisable(GL_LIGHTING);

	for (int row = 0; row < nRows; row += blockSize) {
		for (int col = 0; col < nCols; col += blockSize) {
			float x = -W + (col * 2 * W) / (float)nCols;
			float y = -H + (row * 2 * H) / (float)nRows;

			Vector3f direction;
			float coeff = 0.0001;	// ���������� ���������� ����.

									// ��������� ���� ������.
			color = Color(0, 0, 0);

			// �������� 4 ���� �� ����� ����� � ���������� ������������.
			// ��� ����������� �����������.
			for (int i = 0; i<2; i++) {
				direction = Vector3f(
					-N * n.x + x * u.x + y * v.x + coeff,
					-N * n.y + x * u.y + y * v.y - coeff,
					-N * n.z + x * u.z + y * v.z - coeff
				);
				direction = normalize(direction);
				ray.SetDir(direction);
				color.add(Shade(ray));

				direction = Vector3f(
					-N * n.x + x * u.x + y * v.x + coeff,
					-N * n.y + x * u.y + y * v.y + coeff,
					-N * n.z + x * u.z + y * v.z - coeff
				);
				direction = normalize(direction);
				ray.SetDir(direction);
				color.add(Shade(ray));

				coeff = -coeff;
			}

			// ��������� ����������� ����.
			color = Color(
				color.red / float(4), 
				color.green / float(4), 
				color.blue / float(4)
			);

			// ����������� ���� �������� ���������� ������.
			glColor3f(
				color.red, 
				color.green, 
				color.blue
			);
			glRecti(
				col, 
				row, 
				col + blockSize, 
				row + blockSize
			);
		}
	}
}

Intersection Raytracer::getFirstHit(Ray ray) {
	Intersection inter;	// ������ �����������.
	Intersection best;

	best.numHits = 0;

	// ���� �� ���� ������.
	for (int i = 0; i < scene.vector_Sphere.size(); i++) {
		// ���� ����� ������ ������������.
		if (scene.vector_Sphere[i].display) {
			// ���� ��� ����������.
			if (!scene.vector_Sphere[i].hit(ray, inter))
				continue;

			// ���� ���������� ����.
			// ���������� ����� ������� ����������.
			for (int j = 0; j<inter.numHits; j++)
				inter.hit[j].objectNum = i;

			/*	���� � best ��� ��� ���������� ��� ���������� ���������� �����,
				��� ���������� � best. */
			if (best.numHits == 0 || inter.hit[0].hitTime < best.hit[0].hitTime) // �������� inter � best.
				best.set(inter);
		}
	}

	// ���� �� ���� ����������.
	for (int i = 0; i < scene.vector_Tetrahedron.size(); i++) {
		// ���������� ��� ����������.
		if (scene.vector_Tetrahedron[i].display) {

			// ���� ��� ����������.
			if (!scene.vector_Tetrahedron[i].hit(ray, inter))
				continue;

			//���� ���������� ����
			//���������� ����� ������� ����������
			for (int j = 0; j<inter.numHits; j++) inter.hit[j].objectNum = i;

			/*	���� � best ��� ��� ���������� ��� ���������� ���������� �����,
				��� ���������� � best. */
			if (best.numHits == 0 || inter.hit[0].hitTime < best.hit[0].hitTime) //�������� inter � best
				best.set(inter);
		}
	}

	// ��� ���������.
	if (scene.square.hit(ray, inter)) {
		/*	���� � best ��� ��� ���������� ��� ���������� ���������� �����,
			��� ���������� � best. */
		if (best.numHits == 0 || inter.hit[0].hitTime < best.hit[0].hitTime) //�������� inter � best
			best.set(inter);
	}

	return best;
}

Color Raytracer::Shade(Ray ray) {
	Color color;		// ������������ ���� ������� ����.
	Intersection best;	// ��������� ����������.
	int MAX = 4;		// ������������ ������� ��������.

	// �������� ������ � ������ ����������.
	best = getFirstHit(ray);

	// ����� ������ � ������ ����������.
	HitInfo h = best.hit[0];

	// ����������� �� �����������.
	Vector3f v = Vector3f(
		-ray.dir.x, 
		-ray.dir.y, 
		-ray.dir.z
	);
	v = normalize(v);

	int typeObj = best.hit[0].objectType;	// ��� �������.
	int numObj = best.hit[0].objectNum;		// ����� �������.

	/*	���� ��� ������ ���� ���� ��������, ������� ������� ����
		�� ��������� ���� - ������. */
	if (best.numHits == 0)
		return color;

	// ���������� �� ������.
	if (typeObj == 0) {
		// ���������� ����������� ���� �������.
		color.red = scene.vector_Sphere[numObj].Emission[0];
		color.green = scene.vector_Sphere[numObj].Emission[1];
		color.blue = scene.vector_Sphere[numObj].Emission[2];

		// ������� � ����� ����������.
		Vector3f normal;
		normal = h.hitNormal;
		normal = normalize(normal);

		float eps = 0.0001;	// ����� �����.
		Ray feeler;			// ��� �����.

		feeler.start = h.hitPoint - ray.dir*eps;	// ��������� ����� ����.
		feeler.recurseLevel = 1;

		// ���� �� ���� ���������� ���������.
		for (int i = 0; i < scene.vector_Light.size(); i++) {
			// �������� ������� ���������.
			Color ambientCol = Color(
				scene.vector_Sphere[numObj].Ambient[0] * scene.vector_Light[i].color.red,
				scene.vector_Sphere[numObj].Ambient[1] * scene.vector_Light[i].color.green,
				scene.vector_Sphere[numObj].Ambient[2] * scene.vector_Light[i].color.blue
			);

			color.add(ambientCol.red, ambientCol.green, ambientCol.blue);

			// ��������� ����.
			feeler.dir = scene.vector_Light[i].position - h.hitPoint;	// ����������� ����.

																		// ���� ����� � ����, ��������� � ���������� ���������� �� �����������.
			if (scene.isInShadow(feeler))	continue;

			// ������ �� ����� ���������� �� ���������.
			Vector3f s = scene.vector_Light[i].position - h.hitPoint;
			s = normalize(s);

			// ���� ��������.
			float mDotS = scal(s, normal);

			// ���� ����� ���������� ��������� � �����.
			if (mDotS > 0.0) {
				Color diffuseColor = Color(
					mDotS*scene.vector_Sphere[numObj].Diffuse[0] * scene.vector_Light[i].color.red,
					mDotS*scene.vector_Sphere[numObj].Diffuse[1] * scene.vector_Light[i].color.green,
					mDotS*scene.vector_Sphere[numObj].Diffuse[2] * scene.vector_Light[i].color.blue
				);

				// �������� ��������� ������������.
				color.add(
					diffuseColor.red, 
					diffuseColor.green, 
					diffuseColor.blue
				);
			}

			Vector3f _h = v + s;
			_h = normalize(_h);

			// ���� �����.
			float mDotH = scal(_h, normal);
			if (mDotH > 0) {
				float phong = pow(mDotH, scene.vector_Sphere[numObj].Shininess);
				Color specColor = Color(
					phong*scene.vector_Sphere[numObj].Specular[0] * scene.vector_Light[i].color.red,
					phong*scene.vector_Sphere[numObj].Specular[1] * scene.vector_Light[i].color.green,
					phong*scene.vector_Sphere[numObj].Specular[2] * scene.vector_Light[i].color.blue
				);

				// �������� ���������� ������������.
				color.add(
					specColor.red, 
					specColor.green, 
					specColor.blue
				);
			}

			// ���� ��������� ������������ ������� ��������.
			if (ray.recurseLevel == MAX)
				return color;

			// ���� ������ ���������� ���������.
			if (scene.vector_Sphere[numObj].Shininess > 0.5) {
				Ray reflectedRay;	// ���������� ���.
				Vector3f reflDir = ray.dir - normal * scalar(ray.dir, normal) * 2;	// ����������� ����������� ����.
				reflectedRay.start = h.hitPoint - ray.dir*eps;						// ��������� ����� ����������� ����.
				reflDir = normalize(reflDir);
				reflectedRay.SetDir(reflDir);
				reflectedRay.recurseLevel = ray.recurseLevel + 1;					// ��������� ������� ��������.
				Color c = Color(
					scene.vector_Sphere[numObj].Specular[0], 
					scene.vector_Sphere[numObj].Specular[1], 
					scene.vector_Sphere[numObj].Specular[2]
				);

				// �������� ���������� ����.
				color.add(Shade(reflectedRay), c);
			}
		}
	}

	// ���������� � ����������.
	if (typeObj == 1) {
		// ���������� ����������� ���� �������.
		color.red = scene.vector_Tetrahedron[numObj].Emission[0];
		color.green = scene.vector_Tetrahedron[numObj].Emission[1];
		color.blue = scene.vector_Tetrahedron[numObj].Emission[2];

		// ������� � ����� ����������.
		Vector3f normal;
		normal = h.hitNormal;
		normal = normalize(normal);

		float eps = 0.0001;	// ����� �����.
		Ray feeler;			// ��� �����.

		feeler.start = h.hitPoint - ray.dir*eps;	// ��������� ����� ����.
		feeler.recurseLevel = 1;

		// ���� �� ���� ���������� ���������.
		for (int i = 0; i < scene.vector_Light.size(); i++) {
			// �������� ������� ���������.
			Color ambientCol = Color(
				scene.vector_Tetrahedron[numObj].Ambient[0] * scene.vector_Light[i].color.red,
				scene.vector_Tetrahedron[numObj].Ambient[1] * scene.vector_Light[i].color.green,
				scene.vector_Tetrahedron[numObj].Ambient[2] * scene.vector_Light[i].color.blue
			);

			color.add(
				ambientCol.red, 
				ambientCol.green, 
				ambientCol.blue
			);

			// ��������� ����.
			feeler.dir = scene.vector_Light[i].position - h.hitPoint;	// ����������� ����.

			// ���� ����� � ����, ��������� � ���������� ���������� �� �����������.
			if (scene.isInShadow(feeler))	continue;

			// ������ �� ����� ���������� �� ���������.
			Vector3f s = scene.vector_Light[i].position - h.hitPoint;
			s = normalize(s);

			// ���� ��������.
			float mDotS = scal(s, normal);

			// ���� ����� ���������� ��������� � �����.
			if (mDotS > 0.0) {
				Color diffuseColor = Color(
					mDotS*scene.vector_Tetrahedron[numObj].Diffuse[0] * scene.vector_Light[i].color.red,
					mDotS*scene.vector_Tetrahedron[numObj].Diffuse[1] * scene.vector_Light[i].color.green,
					mDotS*scene.vector_Tetrahedron[numObj].Diffuse[2] * scene.vector_Light[i].color.blue
				);

				// �������� ��������� ������������.
				color.add(diffuseColor.red, diffuseColor.green, diffuseColor.blue);
			}

			Vector3f _h = v + s;
			_h = normalize(_h);

			// ���� �����.
			float mDotH = scal(_h, normal);
			if (mDotH > 0) {
				float phong = pow(mDotH, scene.vector_Tetrahedron[numObj].Shininess);
				Color specColor = Color(
					phong*scene.vector_Tetrahedron[numObj].Specular[0] * scene.vector_Light[i].color.red,
					phong*scene.vector_Tetrahedron[numObj].Specular[1] * scene.vector_Light[i].color.green,
					phong*scene.vector_Tetrahedron[numObj].Specular[2] * scene.vector_Light[i].color.blue
				);

				// �������� ���������� ������������.
				color.add(
					specColor.red, 
					specColor.green, 
					specColor.blue
				);
			}

			// ���� ��������� ������������ ������� ��������.
			if (ray.recurseLevel == MAX)
				return color;

			// ���� ������ ���������� ���������.
			if (scene.vector_Tetrahedron[numObj].Shininess > 0.5) {
				Ray reflectedRay;
				Vector3f reflDir = ray.dir - normal * scalar(ray.dir, normal)*2.0;	// ����������� ����������� ����.
				reflectedRay.start = h.hitPoint - ray.dir*eps;						// ��������� ����� ����������� ����.
				reflDir = normalize(reflDir);
				reflectedRay.SetDir(reflDir);
				reflectedRay.recurseLevel = ray.recurseLevel + 1;
				Color c = Color(
					scene.vector_Tetrahedron[numObj].Specular[0],
					scene.vector_Tetrahedron[numObj].Specular[1], 
					scene.vector_Tetrahedron[numObj].Specular[2]
				);

				// �������� ���������� ������������.
				color.add(Shade(reflectedRay), c);
			}
		}
	}

	// ���������� � ����������.
	if (typeObj == 10) {
		// ���������� ����������� ���� �������.
		color.red = scene.square.Emission[0];
		color.green = scene.square.Emission[1];
		color.blue = scene.square.Emission[2];

		// ������� � ����� ����������.
		Vector3f normal;
		normal = h.hitNormal;
		normal = normalize(normal);

		float eps = 0.0001;	// ����� �����.
		Ray feeler;			// ��� �����.

		feeler.start = h.hitPoint - ray.dir*eps;	// ��������� ����� ����.
		feeler.recurseLevel = 1;

		// ���� �� ���� ���������� ���������.
		for (int i = 0; i < scene.vector_Light.size(); i++) {
			// �������� ������� ���������.
			Color ambientCol = Color(
				scene.square.Ambient[0] * scene.vector_Light[i].color.red,
				scene.square.Ambient[1] * scene.vector_Light[i].color.green,
				scene.square.Ambient[2] * scene.vector_Light[i].color.blue
			);

			color.add(ambientCol.red, ambientCol.green, ambientCol.blue);

			// ��������� ����.
			feeler.dir = scene.vector_Light[i].position - h.hitPoint;	// ����������� ����.

			// ���� ����� � ����, ��������� � ���������� ���������� �� �����������.
			if (scene.isInShadow(feeler))	continue;

			// ������ �� ����� ���������� �� ���������.
			Vector3f s = scene.vector_Light[i].position - h.hitPoint;
			s = normalize(s);

			// ���� ��������.
			float mDotS = scal(s, normal);

			// ���� ����� ���������� ��������� � �����.
			if (mDotS > 0.0) {
				Color diffuseColor = Color(
					mDotS*scene.square.Diffuse[0] * scene.vector_Light[i].color.red,
					mDotS*scene.square.Diffuse[1] * scene.vector_Light[i].color.green,
					mDotS*scene.square.Diffuse[2] * scene.vector_Light[i].color.blue
				);

				// �������� ��������� ������������.
				color.add(
					diffuseColor.red, 
					diffuseColor.green, 
					diffuseColor.blue
				);
			}

			Vector3f _h = v + s;
			_h = normalize(_h);

			// ���� �����.
			float mDotH = scal(_h, normal);
			if (mDotH <= 0) continue;

			float phong = pow(mDotH, scene.square.Shininess);
			Color specColor = Color(
				phong*scene.square.Specular[0] * scene.vector_Light[i].color.red,
				phong*scene.square.Specular[1] * scene.vector_Light[i].color.green,
				phong*scene.square.Specular[2] * scene.vector_Light[i].color.blue
			);

			// �������� ���������� ������������.
			color.add(specColor.red, specColor.green, specColor.blue);

			// ���� ��������� ������������ ������� ��������.
			if (ray.recurseLevel == MAX)
				return color;

			// ���� ������ ���������� ���������.
			if (scene.square.Shininess > 0.5) {
				Ray reflectedRay;
				Vector3f reflDir = ray.dir - normal * scalar(ray.dir, normal) * 2;
				reflectedRay.start = h.hitPoint - ray.dir*eps;
				reflDir = normalize(reflDir);
				reflectedRay.SetDir(reflDir);
				reflectedRay.recurseLevel = ray.recurseLevel + 1;
				Color c = Color(
					scene.square.Specular[0], 
					scene.square.Specular[1], 
					scene.square.Specular[2]
				);

				// �������� ���������� ������������.
				color.add(Shade(reflectedRay), c);
			}
		}
	}
	return color;
}