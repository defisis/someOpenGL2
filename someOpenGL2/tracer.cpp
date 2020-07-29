#include"tracer.h"

/*
Реализация интерфейса класса описанного в файле "tracer.h".
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
	Ray ray;		// Луч.
	Color color;	// Цвет.
	float tetha = 60 * (M_PI / 180.);	// Угол охвата.
	float N = 0.1;						// Расстояние от точки взгляда, до ближней плоскости.
	Vector3f n = camera.Position - camera.View;
	Vector3f u = normal(camera.UpVector, n);
	Vector3f v = normal(n, u);

	// Нормализация с.к., связанной с камерой.
	n = normalize(n);
	u = normalize(u);
	v = normalize(v);

	float H = N * tan(tetha / 2.0);
	float W = H * aspect;

	// Установить координаты начала луча.
	ray.SetStart(eye);

	// Текущий уровень рекурсии.
	ray.recurseLevel = 0;
	nCols = 250;
	nRows = 250;
	// Установки OpenGl для 2D-рисования.
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
			float coeff = 0.0001;	// Коэффицент отклонения луча.

									// Начальный цвет черный.
			color = Color(0, 0, 0);

			// Посылаем 4 луча из одной точки с небольшими отклонениями.
			// Для сглаживания изображения.
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

			// Вычисляем усредненный цвет.
			color = Color(
				color.red / float(4), 
				color.green / float(4), 
				color.blue / float(4)
			);

			// Закрашиваем блок пикселей полученным цветом.
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
	Intersection inter;	// Запись пересечений.
	Intersection best;

	best.numHits = 0;

	// Цикл по всем сферам.
	for (int i = 0; i < scene.vector_Sphere.size(); i++) {
		// Если сфера должна отображаться.
		if (scene.vector_Sphere[i].display) {
			// Если нет соударения.
			if (!scene.vector_Sphere[i].hit(ray, inter))
				continue;

			// Если соударение было.
			// Записываем номер объекта соударения.
			for (int j = 0; j<inter.numHits; j++)
				inter.hit[j].objectNum = i;

			/*	Если в best еще нет соударений или полученное соударение лучше,
				чем соударение в best. */
			if (best.numHits == 0 || inter.hit[0].hitTime < best.hit[0].hitTime) // Копируем inter в best.
				best.set(inter);
		}
	}

	// Цикл по всем тетраэдрам.
	for (int i = 0; i < scene.vector_Tetrahedron.size(); i++) {
		// Аналогично для тетраэдров.
		if (scene.vector_Tetrahedron[i].display) {

			// Если нет соударения.
			if (!scene.vector_Tetrahedron[i].hit(ray, inter))
				continue;

			//если соударение было
			//записываем номер объекта соударения
			for (int j = 0; j<inter.numHits; j++) inter.hit[j].objectNum = i;

			/*	Если в best еще нет соударений или полученное соударение лучше,
				чем соударение в best. */
			if (best.numHits == 0 || inter.hit[0].hitTime < best.hit[0].hitTime) //копируем inter в best
				best.set(inter);
		}
	}

	// Для плоскости.
	if (scene.square.hit(ray, inter)) {
		/*	Если в best еще нет соударений или полученное соударение лучше,
			чем соударение в best. */
		if (best.numHits == 0 || inter.hit[0].hitTime < best.hit[0].hitTime) //копируем inter в best
			best.set(inter);
	}

	return best;
}

Color Raytracer::Shade(Ray ray) {
	Color color;		// Возвращаемый цвет данного луча.
	Intersection best;	// Наилучшее соударение.
	int MAX = 4;		// Максимальный уровень рекурсии.

	// Получить данные о лучшем соударении.
	best = getFirstHit(ray);

	// Копия данных о первом соударении.
	HitInfo h = best.hit[0];

	// Направление на наблюдателя.
	Vector3f v = Vector3f(
		-ray.dir.x, 
		-ray.dir.y, 
		-ray.dir.z
	);
	v = normalize(v);

	int typeObj = best.hit[0].objectType;	// Тип объекта.
	int numObj = best.hit[0].objectNum;		// Номер объекта.

	/*	Если луч прошел мимо всех объектов, вернуть фоновый цвет
		по умолчанию цвет - черный. */
	if (best.numHits == 0)
		return color;

	// Соударение со сферой.
	if (typeObj == 0) {
		// Установить эмиссионный цвет объекта.
		color.red = scene.vector_Sphere[numObj].Emission[0];
		color.green = scene.vector_Sphere[numObj].Emission[1];
		color.blue = scene.vector_Sphere[numObj].Emission[2];

		// Нормаль в точке соударения.
		Vector3f normal;
		normal = h.hitNormal;
		normal = normalize(normal);

		float eps = 0.0001;	// Малое число.
		Ray feeler;			// Щуп теней.

		feeler.start = h.hitPoint - ray.dir*eps;	// Стартовая точка щупа.
		feeler.recurseLevel = 1;

		// Цикл по всем источникам освещения.
		for (int i = 0; i < scene.vector_Light.size(); i++) {
			// Добавить фоновое освещение.
			Color ambientCol = Color(
				scene.vector_Sphere[numObj].Ambient[0] * scene.vector_Light[i].color.red,
				scene.vector_Sphere[numObj].Ambient[1] * scene.vector_Light[i].color.green,
				scene.vector_Sphere[numObj].Ambient[2] * scene.vector_Light[i].color.blue
			);

			color.add(ambientCol.red, ambientCol.green, ambientCol.blue);

			// Обработка тени.
			feeler.dir = scene.vector_Light[i].position - h.hitPoint;	// Направление щупа.

																		// Если точка в тени, диффузная и зеркальная компоненты не учитываются.
			if (scene.isInShadow(feeler))	continue;

			// Вектор от точки соударения до источника.
			Vector3f s = scene.vector_Light[i].position - h.hitPoint;
			s = normalize(s);

			// Член Ламберта.
			float mDotS = scal(s, normal);

			// Если точка соударения повернута к свету.
			if (mDotS > 0.0) {
				Color diffuseColor = Color(
					mDotS*scene.vector_Sphere[numObj].Diffuse[0] * scene.vector_Light[i].color.red,
					mDotS*scene.vector_Sphere[numObj].Diffuse[1] * scene.vector_Light[i].color.green,
					mDotS*scene.vector_Sphere[numObj].Diffuse[2] * scene.vector_Light[i].color.blue
				);

				// Добавить диффузную составляющую.
				color.add(
					diffuseColor.red, 
					diffuseColor.green, 
					diffuseColor.blue
				);
			}

			Vector3f _h = v + s;
			_h = normalize(_h);

			// Член Фонга.
			float mDotH = scal(_h, normal);
			if (mDotH > 0) {
				float phong = pow(mDotH, scene.vector_Sphere[numObj].Shininess);
				Color specColor = Color(
					phong*scene.vector_Sphere[numObj].Specular[0] * scene.vector_Light[i].color.red,
					phong*scene.vector_Sphere[numObj].Specular[1] * scene.vector_Light[i].color.green,
					phong*scene.vector_Sphere[numObj].Specular[2] * scene.vector_Light[i].color.blue
				);

				// Добавить зеркальную составляющую.
				color.add(
					specColor.red, 
					specColor.green, 
					specColor.blue
				);
			}

			// Если достигнут максимальный уровень рекурсии.
			if (ray.recurseLevel == MAX)
				return color;

			// Если объект достаточно блестящий.
			if (scene.vector_Sphere[numObj].Shininess > 0.5) {
				Ray reflectedRay;	// Отраженный луч.
				Vector3f reflDir = ray.dir - normal * scalar(ray.dir, normal) * 2;	// Направление отраженного луча.
				reflectedRay.start = h.hitPoint - ray.dir*eps;						// Стартовая точка отраженного луча.
				reflDir = normalize(reflDir);
				reflectedRay.SetDir(reflDir);
				reflectedRay.recurseLevel = ray.recurseLevel + 1;					// Увеличить уровень рекурсии.
				Color c = Color(
					scene.vector_Sphere[numObj].Specular[0], 
					scene.vector_Sphere[numObj].Specular[1], 
					scene.vector_Sphere[numObj].Specular[2]
				);

				// Добавить отраженный свет.
				color.add(Shade(reflectedRay), c);
			}
		}
	}

	// Соударение с тетраэдром.
	if (typeObj == 1) {
		// Установить эмиссионный цвет объекта.
		color.red = scene.vector_Tetrahedron[numObj].Emission[0];
		color.green = scene.vector_Tetrahedron[numObj].Emission[1];
		color.blue = scene.vector_Tetrahedron[numObj].Emission[2];

		// Нормаль в точке соударения.
		Vector3f normal;
		normal = h.hitNormal;
		normal = normalize(normal);

		float eps = 0.0001;	// Малое число.
		Ray feeler;			// Щуп теней.

		feeler.start = h.hitPoint - ray.dir*eps;	// Стартовая точка щупа.
		feeler.recurseLevel = 1;

		// Цикл по всем источникам освещения.
		for (int i = 0; i < scene.vector_Light.size(); i++) {
			// Добавить фоновое освещение.
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

			// Обработка тени.
			feeler.dir = scene.vector_Light[i].position - h.hitPoint;	// Направление щупа.

			// Если точка в тени, диффузная и зеркальная компоненты не учитываются.
			if (scene.isInShadow(feeler))	continue;

			// Вектор от точки соударения до источника.
			Vector3f s = scene.vector_Light[i].position - h.hitPoint;
			s = normalize(s);

			// Член Ламберта.
			float mDotS = scal(s, normal);

			// Если точка соударения повернута к свету.
			if (mDotS > 0.0) {
				Color diffuseColor = Color(
					mDotS*scene.vector_Tetrahedron[numObj].Diffuse[0] * scene.vector_Light[i].color.red,
					mDotS*scene.vector_Tetrahedron[numObj].Diffuse[1] * scene.vector_Light[i].color.green,
					mDotS*scene.vector_Tetrahedron[numObj].Diffuse[2] * scene.vector_Light[i].color.blue
				);

				// Добавить диффузную составляющую.
				color.add(diffuseColor.red, diffuseColor.green, diffuseColor.blue);
			}

			Vector3f _h = v + s;
			_h = normalize(_h);

			// Член Фонга.
			float mDotH = scal(_h, normal);
			if (mDotH > 0) {
				float phong = pow(mDotH, scene.vector_Tetrahedron[numObj].Shininess);
				Color specColor = Color(
					phong*scene.vector_Tetrahedron[numObj].Specular[0] * scene.vector_Light[i].color.red,
					phong*scene.vector_Tetrahedron[numObj].Specular[1] * scene.vector_Light[i].color.green,
					phong*scene.vector_Tetrahedron[numObj].Specular[2] * scene.vector_Light[i].color.blue
				);

				// Добавить зеркальную составляющую.
				color.add(
					specColor.red, 
					specColor.green, 
					specColor.blue
				);
			}

			// Если достигнут максимальный уровень рекурсии.
			if (ray.recurseLevel == MAX)
				return color;

			// Если объект достаточно блестящий.
			if (scene.vector_Tetrahedron[numObj].Shininess > 0.5) {
				Ray reflectedRay;
				Vector3f reflDir = ray.dir - normal * scalar(ray.dir, normal)*2.0;	// Направление отраженного луча.
				reflectedRay.start = h.hitPoint - ray.dir*eps;						// Стартовая точка отраженного луча.
				reflDir = normalize(reflDir);
				reflectedRay.SetDir(reflDir);
				reflectedRay.recurseLevel = ray.recurseLevel + 1;
				Color c = Color(
					scene.vector_Tetrahedron[numObj].Specular[0],
					scene.vector_Tetrahedron[numObj].Specular[1], 
					scene.vector_Tetrahedron[numObj].Specular[2]
				);

				// Добавить отраженную составляющую.
				color.add(Shade(reflectedRay), c);
			}
		}
	}

	// Соударение с плоскостью.
	if (typeObj == 10) {
		// Установить эмиссионный цвет объекта.
		color.red = scene.square.Emission[0];
		color.green = scene.square.Emission[1];
		color.blue = scene.square.Emission[2];

		// Нормаль в точке соударения.
		Vector3f normal;
		normal = h.hitNormal;
		normal = normalize(normal);

		float eps = 0.0001;	// Малое число.
		Ray feeler;			// Щуп теней.

		feeler.start = h.hitPoint - ray.dir*eps;	// Стартовая точка щупа.
		feeler.recurseLevel = 1;

		// Цикл по всем источникам освещения.
		for (int i = 0; i < scene.vector_Light.size(); i++) {
			// Добавить фоновое освещение.
			Color ambientCol = Color(
				scene.square.Ambient[0] * scene.vector_Light[i].color.red,
				scene.square.Ambient[1] * scene.vector_Light[i].color.green,
				scene.square.Ambient[2] * scene.vector_Light[i].color.blue
			);

			color.add(ambientCol.red, ambientCol.green, ambientCol.blue);

			// Обработка тени.
			feeler.dir = scene.vector_Light[i].position - h.hitPoint;	// Направление щупа.

			// Если точка в тени, диффузная и зеркальная компоненты не учитываются.
			if (scene.isInShadow(feeler))	continue;

			// Вектор от точки соударения до источника.
			Vector3f s = scene.vector_Light[i].position - h.hitPoint;
			s = normalize(s);

			// Член Ламберта.
			float mDotS = scal(s, normal);

			// Если точка соударения повернута к свету.
			if (mDotS > 0.0) {
				Color diffuseColor = Color(
					mDotS*scene.square.Diffuse[0] * scene.vector_Light[i].color.red,
					mDotS*scene.square.Diffuse[1] * scene.vector_Light[i].color.green,
					mDotS*scene.square.Diffuse[2] * scene.vector_Light[i].color.blue
				);

				// Добавить диффузную составляющую.
				color.add(
					diffuseColor.red, 
					diffuseColor.green, 
					diffuseColor.blue
				);
			}

			Vector3f _h = v + s;
			_h = normalize(_h);

			// Член Фонга.
			float mDotH = scal(_h, normal);
			if (mDotH <= 0) continue;

			float phong = pow(mDotH, scene.square.Shininess);
			Color specColor = Color(
				phong*scene.square.Specular[0] * scene.vector_Light[i].color.red,
				phong*scene.square.Specular[1] * scene.vector_Light[i].color.green,
				phong*scene.square.Specular[2] * scene.vector_Light[i].color.blue
			);

			// Добавить зеркальную составляющую.
			color.add(specColor.red, specColor.green, specColor.blue);

			// Если достигнут максимальный уровень рекурсии.
			if (ray.recurseLevel == MAX)
				return color;

			// Если объект достаточно блестящий.
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

				// Добавить отраженную составляющую.
				color.add(Shade(reflectedRay), c);
			}
		}
	}
	return color;
}