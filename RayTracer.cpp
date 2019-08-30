#include "RayTracer.h"

#define RECURSION_THRESHOLD 1e-4
#define RAY_EPSILON 2e-5

static SceneColor ambientLighting(FlatSceneNode* closestObj);

RayTracer::RayTracer()
{
	flatSceneTree = NULL;
	parser = NULL;

	numLights = 0;
	lightData = NULL;
	recurseDepth = 0;

	cube = NULL;
	cylinder = NULL;
	cone = NULL;
	sphere = NULL;
}

RayTracer::RayTracer(FlatSceneTree* a, SceneParser* b, Cube* d, Cylinder* e, 
    Cone* f, Sphere* g)
{
	int i;

	flatSceneTree = a;
	parser = b;

	cube = d;
	cylinder = e;
	cone = f;
	sphere = g;

	recurseDepth = 0; // Check if right

	// Load light data
    numLights = parser->getNumLights();
    lightData = new SceneLightData[numLights];
    for (i = 0; i < numLights; i++)
    {
        parser->getLightData(i, lightData[i]);
    }
    std::cout << "Light Data Allocated" << std::endl;

    // Load global data
    parser->getGlobalData(globalData);

    std::cout << "RayTracer Constructed" << std::endl;
}

RayTracer::~RayTracer()
{
	delete[] lightData;
	std::cout << "[RayTracer] LightData deleted" << std::endl;
	std::cout << "[RayTracer] RayTracer destructor called" << std::endl;
}

double
RayTracer::rayCast(glm::vec3 eye, glm::vec3 ray, FlatSceneNode* &closestObj)
{
	int numNodes, typePrimitive, i; // Used for each intersects
	glm::mat4 transformMat; // Used for each intersects
	double d; // Ray's distance to each object, used for each iteration
	double t = -1.0; // Ray's distance to the closest object
	bool hit = false;  // Whether the ray hit anything or not

	numNodes = flatSceneTree->getSize();

    for (i = 0; i < numNodes; i++)
    {
        typePrimitive = flatSceneTree->getNode(i)->getPrimitive()->type;
        transformMat = flatSceneTree->getNode(i)->getTransformMat();
        
        switch (typePrimitive)
        {
            case SHAPE_CUBE: d = cube->intersect(eye, ray, transformMat); break;
            case SHAPE_SPHERE: d = sphere->intersect(eye, ray, transformMat); break;
            case SHAPE_CYLINDER: d = cylinder->intersect(eye, ray, transformMat); break;
            case SHAPE_CONE: d = cone->intersect(eye, ray, transformMat); break;
            default: std::cout << "SHAPE ERROR" << std::endl; exit(1); break;
        }        

        if (d > 0.0)
        {
            if (!hit)
            {
                t = d;
                closestObj = flatSceneTree->getNode(i); 
                hit = true;
            }
            else
            {
            	if (d < t)
            	{
            		t = d;
            		closestObj = flatSceneTree->getNode(i);
            	}
            }
        }
    }

    return t;
}

SceneColor
RayTracer::rayTrace(glm::vec3 eye, glm::vec3 ray, double t, 
    FlatSceneNode* closestObj, int iteration)
{
    //std::cout << "RayTrace iteration" << iteration << std::endl;

    OBJ_TYPE typePrimitive;
    SceneColor ambientColor, diffSpecColor, reflRefrColor, sumColor;
    glm::mat4 transformMat; // Regular transform matrix from obj to world coord
    glm::mat4 normObj2World; // Special transformation matrix for normals
    glm::vec3 normal; // Initially in object, should transform to world coord

    //std::cout << "getPrmit" << std::endl;
    typePrimitive = closestObj->getPrimitive()->type;
    transformMat = closestObj->getTransformMat();

    //std::cout << "swtit" << std::endl;
    switch (typePrimitive)
    {
        case SHAPE_CUBE: normal = cube->intersect_normal(eye, ray, transformMat, t); break;
        case SHAPE_SPHERE: normal = sphere->intersect_normal(eye, ray, transformMat, t); break;
        case SHAPE_CYLINDER: normal = cylinder->intersect_normal(eye, ray, transformMat, t); break;
        case SHAPE_CONE: normal = cone->intersect_normal(eye, ray, transformMat, t); break;
        default: std::cout << "SHAPE ERROR" << std::endl; break; 
    }

    //std::cout << "get normal" << std::endl;
    // Get normal for intersect in object switch

    // Transform normal from object to world space
    normObj2World = glm::transpose(glm::inverse(transformMat));
    normal = glm::normalize(glm::vec3(normObj2World * glm::vec4(normal, 1.0)));

    //std::cout << "lighting" << std::endl;
    // Phong lighting
    ambientColor = ambientLighting(closestObj);
    diffSpecColor = diffuseSpecularLighting(eye, ray, t, normal, closestObj);
    if (!(iteration >= recurseDepth))
    {
        reflRefrColor = reflectRefractLighting(eye, ray, t, normal, closestObj,
            iteration);
    }
    else
    {
        reflRefrColor.r = 0.0;
        reflRefrColor.g = 0.0;
        reflRefrColor.b = 0.0;
    }
    
    sumColor.r = ambientColor.r + diffSpecColor.r + reflRefrColor.r;
    sumColor.g = ambientColor.g + diffSpecColor.g + reflRefrColor.g;
    sumColor.b = ambientColor.b + diffSpecColor.b + reflRefrColor.b;

    sumColor = mapTexture(eye, ray, t, closestObj, sumColor);

    sumColor.r = sumColor.r > 1.0 ? 1.0 : sumColor.r;
    sumColor.g = sumColor.g > 1.0 ? 1.0 : sumColor.g;
    sumColor.b = sumColor.b > 1.0 ? 1.0 : sumColor.b;

    return sumColor;
}

SceneColor
RayTracer::mapTexture(glm::vec3 eye, glm::vec3 ray, double t, 
    FlatSceneNode* closestObj, SceneColor sumColor)
{
    SceneColor texture;
    OBJ_TYPE typePrimitive;
    float blend;

    if (closestObj->hasTexture)
    {
        blend = closestObj->getPrimitive()->material.blend;
        typePrimitive = closestObj->getPrimitive()->type;

        //std::cout << typePrimitive << std::endl;

        switch (typePrimitive)
        {
            case SHAPE_CUBE: 
                texture = cube->mapTexture(eye, ray, t, closestObj);
                break;
            case SHAPE_SPHERE:
                texture = sphere->mapTexture(eye, ray, t, closestObj);
                break;
            case SHAPE_CYLINDER:
                texture = cylinder->mapTexture(eye, ray, t, closestObj);
                break;
            case SHAPE_CONE: 
                texture = cone->mapTexture(eye, ray, t, closestObj);
                break;
            default: std::cout << "SHAPE ERROR" << std::endl; break; 
        }

        sumColor.r = sumColor.r * (1.0 - blend) + texture.r * blend;
        sumColor.g = sumColor.g * (1.0 - blend) + texture.g * blend;
        sumColor.b = sumColor.b * (1.0 - blend) + texture.b * blend;
    }

    return sumColor;
}

void
RayTracer::setRecurseDepth(int depth)
{
	recurseDepth = depth;
    std::cout << "[RayTracer] Recurse depth set to " << depth << std::endl;
}

int
RayTracer::getRecurseDepth()
{
	return recurseDepth;
}

SceneColor
RayTracer::ambientLighting(FlatSceneNode* closestObj)
{
    SceneColor ambientColor, oal;

    oal = closestObj->getPrimitive()->material.cAmbient;

    ambientColor.r = globalData.ka * oal.r;
    ambientColor.g = globalData.ka * oal.g;
    ambientColor.b = globalData.ka * oal.b;

    return ambientColor;
}

SceneColor
RayTracer::diffuseSpecularLighting(glm::vec3 eye, glm::vec3 ray, double t,
    glm::vec3 normal, FlatSceneNode* closestObj)
{
    SceneColor sumColor, diffColor, specColor, odl, osl;
    LightType lightType;
    glm::vec3 point, _point, light, _light, rLight;
    float dpNL, dpRV, f, dpRVf;
    double t_Light, _t;
    int i;

    odl = closestObj->getPrimitive()->material.cDiffuse;
    osl = closestObj->getPrimitive()->material.cSpecular;
    f = closestObj->getPrimitive()->material.shininess;

    point = eye + ((float)t * ray);
    _point = point - ((float)RAY_EPSILON * ray); // Move intersect point a bit

    sumColor.r = sumColor.g = sumColor.b = 0.0f;

    for (i = 0; i < numLights; i++)
    {
        lightType = lightData[i].type;

        switch (lightType)
        {
            case LIGHT_POINT:
                light = glm::normalize(lightData[i].pos - point);
                _light = glm::normalize(lightData[i].pos - _point);
                t_Light = glm::length(lightData[i].pos - _point);
                break;
            case LIGHT_DIRECTIONAL:
                light = glm::normalize(-lightData[i].dir);
                _light = light;
                t_Light = glm::length(-lightData[i].dir);
                break;
            default:
                std::cout << "LIGHT TYPE ERROR(?)" << std::endl;
                break;
        }

        // Shadow check
        _t = rayCast(_point, _light, closestObj);

        if (_t < 0.0 || (t_Light < _t))
        {
            dpNL = glm::dot(normal, light);
            rLight = glm::normalize(light - (2.0f * dpNL * normal));
            dpRV = glm::dot(rLight, ray);

            dpNL = dpNL < 0.0 ? 0.0 : dpNL;
            dpRV = dpRV < 0.0 ? 0.0 : dpRV;

            dpRVf = glm::pow(dpRV, f);
            diffColor.r = globalData.kd * odl.r * dpNL;
            diffColor.g = globalData.kd * odl.g * dpNL;
            diffColor.b = globalData.kd * odl.b * dpNL;
            specColor.r = globalData.ks * osl.r * dpRVf;
            specColor.g = globalData.ks * osl.g * dpRVf;
            specColor.b = globalData.ks * osl.b * dpRVf;
            sumColor.r += lightData[i].color.r * (diffColor.r + specColor.r);
            sumColor.g += lightData[i].color.g * (diffColor.g + specColor.g);
            sumColor.b += lightData[i].color.b * (diffColor.b + specColor.b);
        }
    }

    return sumColor;
}

SceneColor
RayTracer::reflectRefractLighting(glm::vec3 eye, glm::vec3 ray, double t,
    glm::vec3 normal, FlatSceneNode* closestObj, int iteration)
{
    glm::vec3 point, _point, reflectRay, refractRay;
    float ior, eta, c1, c2;
    double t_Reflect, t_Refract;
    SceneColor sumColor, reflectColor, refractColor, osl, otl;

    sumColor.r = sumColor.g = sumColor.b = 0.0;

    osl = closestObj->getPrimitive()->material.cSpecular;
    otl = closestObj->getPrimitive()->material.cTransparent;
    ior = closestObj->getPrimitive()->material.ior;

    point = eye + ((float)t * ray);
    _point = point - ((float)RAY_EPSILON * ray);

    // Reflection
    reflectRay = glm::normalize(ray - (2.0f * glm::dot(ray, normal) * normal));
    t_Reflect = rayCast(_point, reflectRay, closestObj);

    if (t_Reflect > 0.0)
    {
        reflectColor = rayTrace(_point, reflectRay, t_Reflect, closestObj, 
            iteration + 1);

        sumColor.r = globalData.ks * osl.r * reflectColor.r;
        sumColor.g = globalData.ks * osl.g * reflectColor.g;
        sumColor.b = globalData.ks * osl.b * reflectColor.b;
    }

    //Refraction
    eta = 1.0f / ior;
    c1 = glm::dot(normal, ray);
    c2 = glm::sqrt(1.0f - (eta * eta) * (1.0f - (c1 * c1)));

    if (ior > 0.0f)
    {
        refractRay = (eta * ray) + (((eta * c1) - c2) * normal);
        t_Refract = rayCast(_point, refractRay, closestObj);

        if (t_Refract > 0.0)
        {
            refractColor = rayTrace(_point, refractRay, t_Refract, closestObj,
                iteration + 1);

            sumColor.r = globalData.kt * otl.r * refractColor.r;
            sumColor.g = globalData.kt * otl.g * refractColor.g;
            sumColor.b = globalData.kt * otl.b * refractColor.b;
        }
    }

    return sumColor;
}

// Source: https://www.scratchapixel.com/lessons/3d-basic-rendering
// /introduction-to-shading/reflection-refraction-fresnel