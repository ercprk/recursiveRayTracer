#ifndef SHAPE_H
#define SHAPE_H

#include <iostream>
#include <FL/gl.h>
#include <FL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "FlatSceneTree.h"
#include "scene/SceneParser.h"

const float EPSILON = 1e-5;
#define PI glm::pi<float>()  //PI is now a constant for 3.14159....
#define IN_RANGE(a,b) (((a>(b-EPSILON))&&(a<(b+EPSILON)))?1:0)

// Bound, offset, range, limit, ...
const float OFFSET = 0.5f;
const float RADIUS = 0.5f;
const double DNE = -1.0; // Does Not Exist (DNE)
const double BOUND = 0.5;

class Shape
{

public:

	Shape()
	{

	};

	~Shape() {};

	virtual OBJ_TYPE getType() = 0;
	virtual SceneColor mapTexture(glm::vec3 eye, glm::vec3 ray, double t, FlatSceneNode* node) {SceneColor color; return color;};
	virtual double intersect(glm::vec3 eyePointWorld, glm::vec3 rayWorld, glm::mat4 transformMat) {return 0.0;};
	virtual glm::vec3 intersect_normal(glm::vec3 eyePointWorld, glm::vec3 rayWorld, glm::mat4 transformMat, double t) {return glm::vec3(0.0);};

protected:

	// Intersects the ray with the plane, (NOTE: doesn't check the bound)
	double intersect_plane(glm::vec3 eyePointObject, glm::vec3 rayObject, glm::vec3 centerPoint)
	{
		glm::vec3 planeNormal = glm::normalize(centerPoint);

		if (IN_RANGE(glm::dot(rayObject, planeNormal), 0.0f))
		{
			return DNE;
		}
		else
		{
			return (double)glm::dot(centerPoint - eyePointObject, planeNormal) / (double)glm::dot(rayObject, planeNormal);
		}
	};

	bool within_bounds(double n)
	{
		double max = +BOUND;
		double min = -BOUND;

		return ((n < max || IN_RANGE(n, max)) && (n > min || IN_RANGE(n, min))) ? 1 : 0;
	};

	bool within_circle(double x, double z)
	{
		double max = RADIUS * RADIUS;
		double r = (x * x) + (z * z);

		return (r < max || IN_RANGE(r, max)) ? 1 : 0;
	};
};

#endif
