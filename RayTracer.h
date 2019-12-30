#ifndef RAYTRACER_H
#define RAYTRACER_H

#define GLM_ENABLE_EXPERIMENTAL

#include "Shape.h"
#include "Cube.h"
#include "Cylinder.h"
#include "Cone.h"
#include "Sphere.h"

//#include "FlatSceneTree.h"
#include "scene/SceneParser.h"

#include <glm/gtx/string_cast.hpp>

class RayTracer
{

public:

	FlatSceneTree* flatSceneTree;
	SceneParser* parser;

	OBJ_TYPE objType;
	Cube* cube;
	Cylinder* cylinder;
	Cone* cone;
	Sphere* sphere;
	Shape* shape;

	RayTracer();
	RayTracer(FlatSceneTree* a, SceneParser* b, Cube* d,
		Cylinder* e, Cone* f, Sphere* g);
	~RayTracer();

	glm::vec3 primaryRay();
	double rayCast(glm::vec3 eye, glm::vec3 ray, FlatSceneNode* &closestObj);
	SceneColor rayTrace(glm::vec3 eye, glm::vec3 ray, double t,
		FlatSceneNode* closestObj, int iteration);

	void setRecurseDepth(int depth);
	int getRecurseDepth();

private:

	SceneColor ambientLighting(FlatSceneNode* closestObj);
	SceneColor diffuseSpecularLighting(glm::vec3 eye, glm::vec3 ray, double t,
		glm::vec3 normal, FlatSceneNode* closestObj);
	SceneColor reflectRefractLighting(glm::vec3 eye, glm::vec3 ray, double t,
		glm::vec3 normal, FlatSceneNode* closestObj, int iteration);
	SceneColor mapTexture(glm::vec3 eye, glm::vec3 ray, double t,
		FlatSceneNode* closestObj, SceneColor sumColor);
	int recurseDepth;
	int numLights;
	SceneLightData* lightData;
	SceneGlobalData globalData;
};

#endif
