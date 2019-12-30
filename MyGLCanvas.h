#pragma once

#ifndef MYGLCANVAS_H
#define MYGLCANVAS_H

#define GLM_ENABLE_EXPERIMENTAL

#include <FL/gl.h>
#include <FL/glut.h>
#include <FL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <time.h>
#include <iostream>

#include "Camera.h"
#include "RayTracer.h"
//#include "FlatSceneTree.h"
#include "scene/SceneParser.h"

#include "Shape.h"
#include "Cube.h"
#include "Cylinder.h"
#include "Cone.h"
#include "Sphere.h"

class MyGLCanvas : public Fl_Gl_Window
{

public:

	glm::vec3 rotVec;
	glm::vec3 eyePosition;
	GLubyte* pixels;

	OBJ_TYPE objType;
	Cube* cube;
	Cylinder* cylinder;
	Cone* cone;
	Sphere* sphere;
	Shape* shape;

	int isectOnly;
	int recurseDepth;
	float scale;

	Camera* camera;
	RayTracer* rayTracer;
	SceneParser* parser;
	FlatSceneTree* flatSceneTree;

	MyGLCanvas(int x, int y, int w, int h, const char *l = 0);
	~MyGLCanvas();
	void renderShape(OBJ_TYPE type);
	void loadSceneFile(const char* filenamePath);
	void renderScene();

private:

	void setpixel(GLubyte* buf, int x, int y, int r, int g, int b);
	void draw();
	int handle(int e);
	void resize(int x, int y, int w, int h);
	void updateCamera(int width, int height);
    void updateRayTracer(int recurseDepth);
};

#endif //!MYGLCANVAS_H
