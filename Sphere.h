#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"

class Sphere : public Shape
{

public:

    Sphere() 
    {
        std::cout << "[Sphere] Constructor called" << std::endl;
    };

    ~Sphere()
    {
        std::cout << "[Sphere] Destructor called" << std::endl;
    };

    OBJ_TYPE getType()
    {
    	return SHAPE_SPHERE;
    };

    SceneColor mapTexture(glm::vec3 eye, glm::vec3 ray, double t_, 
        FlatSceneNode* node)
    {   
        std::cout << "in sphere maptexture" << std::endl;

        glm::mat4 invTransformMat = glm::inverse(node->getTransformMat());
        eye = glm::vec3(invTransformMat * glm::vec4(eye, 1.0));
        ray = glm::vec3(invTransformMat * glm::vec4(ray, 0.0));
        SceneColor texture;

        glm::vec3 point = -(eye + ((float)t_ * ray));
        int s, t;

        double theta = -atan2(point[2], point[0]);
          
        while (theta < 0)
        {
            theta += 2 * PI;
        }

        double u = theta / (2.*PI);
        double phi = asin(point[Y] / 0.5);
        double v = phi / PI + .5;

        if (IN_RANGE(v, 0) ||  IN_RANGE(v, 1))
        {
            u = 0.5;
        }

        s = (int)(node->textureWidth * u * node->repeatU) % node->textureWidth;
        t = (int)(node->textureHeight * v * node->repeatV) % node->textureHeight;

        texture.r = node->texture[t][s].r;
        texture.g = node->texture[t][s].g;
        texture.b = node->texture[t][s].b;

        return texture;
    };

    glm::vec3 intersect_normal(glm::vec3 eyePointWorld, glm::vec3 rayWorld, glm::mat4 transformMat, double t)
    {
        glm::mat4 invTransformMat = glm::inverse(transformMat);
        glm::vec3 eyePointObject = glm::vec3(invTransformMat * glm::vec4(eyePointWorld, 1.0));
        glm::vec3 rayObject = glm::vec3(invTransformMat * glm::vec4(rayWorld, 0.0));

        glm::vec3 pointObject = eyePointObject + ((float)t * rayObject);
        glm::vec3 normalObject = glm::normalize(pointObject);

        return normalObject;
    };

    double intersect(glm::vec3 eyePointWorld, glm::vec3 rayWorld, glm::mat4 transformMat)
    {
        glm::mat4 invTransformMat = glm::inverse(transformMat);
        glm::vec3 eyePointObject = glm::vec3(invTransformMat * glm::vec4(eyePointWorld, 1.0));
        glm::vec3 rayObject = glm::vec3(invTransformMat * glm::vec4(rayWorld, 0.0));

        double a = glm::dot(rayObject, rayObject);
        double b = 2.0 * glm::dot(eyePointObject, rayObject);
        double c = glm::dot(eyePointObject, eyePointObject) - (RADIUS * RADIUS);

        double discriminant = (b * b) - (4.0 * a * c);
        
        if (discriminant < 0.0)
        {
            return DNE;
        }
        else if (IN_RANGE(discriminant, 0.0))
        {
            return -b / (2.0 * a);
        }
        else
        {
            double t1 = (-b + glm::sqrt(discriminant)) / (2.0 * a);
            double t2 = (-b - glm::sqrt(discriminant)) / (2.0 * a);

            return fmin(t1, t2);
        }
    };

private:

};

#endif