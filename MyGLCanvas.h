#pragma once

#ifndef MYGLCANVAS_H
#define MYGLCANVAS_H

#include <FL/gl.h>
#include <FL/glut.h>
#include <FL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#include <iostream>

#include "Shape.h"
#include "Cube.h"
#include "Cylinder.h"
#include "Cone.h"
#include "Sphere.h"

#include "Camera.h"
#include "scene/SceneParser.h"
#include "RayTracer.h"

class MyGLCanvas : public Fl_Gl_Window {
public:
	glm::vec3 rotVec;
	glm::vec3 eyePosition;
	GLubyte* pixels;// = NULL;

	int isectOnly;
	int recurseDepth;
	float scale;

	OBJ_TYPE objType;
	Cube* cube;
	Cylinder* cylinder;
	Cone* cone;
	Sphere* sphere;
	Shape* shape;

	Camera* camera;
	SceneParser* parser;
	FlatSceneTree* flatSceneTree;
	RayTracer* rayTracer;

	MyGLCanvas(int x, int y, int w, int h, const char *l = 0);
	~MyGLCanvas();
	void renderShape(OBJ_TYPE type);
	//void setSegments();
	void loadSceneFile(const char* filenamePath);
	void renderScene();

private:
	void setpixel(GLubyte* buf, int x, int y, int r, int g, int b);

	void draw();

	int handle(int);
	void resize(int x, int y, int w, int h);
	void updateCamera(int width, int height);
	void updateRayTracer(int recurseDepth);
};

#endif // !MYGLCANVAS_H
