#ifndef CUBE_H
#define CUBE_H

#include "Shape.h"

class Cube : public Shape
{

public:

    Cube()
    {
        std::cerr << "[Cube] Constructor called" << std::endl;
    };

    ~Cube()
    {
        std::cerr << "[Cube] Destructor called" << std::endl;
    };

    OBJ_TYPE getType()
    {
        return SHAPE_CUBE;
    };

    SceneColor mapTexture(glm::vec3 eye, glm::vec3 ray, double t_,
        FlatSceneNode* node)
    {
        std::cout << "in cube maptexture" << std::endl;

        glm::mat4 invTransformMat = glm::inverse(node->getTransformMat());
        eye = glm::vec3(invTransformMat * glm::vec4(eye, 1.0));
        ray = glm::vec3(invTransformMat * glm::vec4(ray, 0.0));
        SceneColor texture;

        glm::vec3 point = eye + ((float)t_ * ray);
        int s, t;

        if (IN_RANGE(point[X], .5)) {
            s = (int)(node->textureWidth * node->repeatU * (-point[2] + 0.5)) % node->textureWidth;
            t = (int)(node->textureHeight * node->repeatV * (-point[1] + 0.5)) % node->textureHeight;
        }
        else if (IN_RANGE(point[X], -.5)) {
            s = (int)(node->textureWidth * node->repeatU * (point[2] + 0.5)) % node->textureWidth;
            t = (int)(node->textureHeight * node->repeatV * (-point[1] + 0.5)) % node->textureHeight;
        }
        else if (IN_RANGE(point[Y], .5)) {
            s = (int)(node->textureWidth * node->repeatU * (point[0] + 0.5)) % node->textureWidth;
            t = (int)(node->textureHeight * node->repeatV * (point[2] + 0.5)) % node->textureHeight;
        }
        else if (IN_RANGE(point[Y], -.5)) {
            s = (int)(node->textureWidth * node->repeatU * (point[0] + 0.5)) % node->textureWidth;
            t = (int)(node->textureHeight * node->repeatV * (point[2] + 0.5)) % node->textureHeight;
        }
        else if (IN_RANGE(point[Z], .5)) {
            s = (int)(node->textureWidth * node->repeatU * (point[0] + 0.5)) % node->textureWidth;
            t = (int)(node->textureHeight * node->repeatV * (-point[1] + 0.5)) % node->textureHeight;
        }
        else if (IN_RANGE(point[Z], -.5)) {
            s = (int)(node->textureWidth * node->repeatU * (-point[0] + 0.5)) % node->textureWidth;
            t = (int)(node->textureHeight * node->repeatV * (-point[1] + 0.5)) % node->textureHeight;
        } else {
            std::cerr << "cube mapTexture no side" << std::endl;
            std::cerr << node->getPrimitive()->type << std::endl;

            SceneColor l;
            l.r = l.g = l.b = 0.0;
            return l;
        }

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

       	double x = eyePointObject[X] + (t * rayObject[X]);
       	double y = eyePointObject[Y] + (t * rayObject[Y]);
       	double z = eyePointObject[Z] + (t * rayObject[Z]);

       	//printf("%f %f %f\n", x, y, z);

        if (IN_RANGE(x, 0.5))
        {
        	return glm::vec3(1.0, 0.0, 0.0);
        }
        else if (IN_RANGE(x, -0.5))
        {
        	return glm::vec3(-1.0, 0.0, 0.0);
        }
        else if (IN_RANGE(y, 0.5))
        {
        	return glm::vec3(0.0, 1.0, 0.0);
        }
        else if (IN_RANGE(y, -0.5))
        {
        	return glm::vec3(0.0, -1.0, 0.0);
        }
        else if (IN_RANGE(z, 0.5))
        {
        	return glm::vec3(0.0, 0.0, 1.0);
        }
        else if (IN_RANGE(z, -0.5))
        {
        	return glm::vec3(0.0, 0.0, -1.0);
        }
        else
        {
        	std::cout << "somthing went wrong with intersectN in cube" << std::endl;
        	return glm::vec3(0.0);
        }
    };

    double intersect(glm::vec3 eyePointWorld, glm::vec3 rayWorld, glm::mat4 transformMat)
    {
        glm::mat4 invTransformMat = glm::inverse(transformMat);
        glm::vec3 eyePointObject = glm::vec3(invTransformMat * glm::vec4(eyePointWorld, 1.0));
        glm::vec3 rayObject = glm::vec3(invTransformMat * glm::vec4(rayWorld, 0.0));

        // Ray-Plane Intersects
        double t[6];
        t[0] = intersect_plane(eyePointObject, rayObject, glm::vec3(+0.5, 0.0, 0.0));
        t[1] = intersect_plane(eyePointObject, rayObject, glm::vec3(-0.5, 0.0, 0.0));
        t[2] = intersect_plane(eyePointObject, rayObject, glm::vec3(0.0, +0.5, 0.0));
        t[3] = intersect_plane(eyePointObject, rayObject, glm::vec3(0.0, -0.5, 0.0));
        t[4] = intersect_plane(eyePointObject, rayObject, glm::vec3(0.0, 0.0, +0.5));
        t[5] = intersect_plane(eyePointObject, rayObject, glm::vec3(0.0, 0.0, -0.5));

        double t_min = -1.0;
        bool hit = false;

        // Check for bounds
        for (int i = 0; i < 6; i++)
        {
        	double _t = t[i];

        	if (_t > 0.0 || IN_RANGE(_t, 0.0))
        	{
        		double x = eyePointObject[X] + (_t * rayObject[X]);
        		double y = eyePointObject[Y] + (_t * rayObject[Y]);
        		double z = eyePointObject[Z] + (_t * rayObject[Z]);

        		bool is_valid = false;

        		switch(i)
        		{
        			case 0:
        			case 1:
        				is_valid = within_bounds(y) && within_bounds(z);
        				break;
        			case 2:
        			case 3:
        				is_valid = within_bounds(x) && within_bounds(z);
        				break;
        			case 4:
        			case 5:
        				is_valid = within_bounds(x) && within_bounds(y);
        				break;
        			default:
        				break;
        		}

        		if (is_valid)
        		{
        			if (!hit)
        			{
        				t_min = _t;
        				hit = true;
        			}
        			else
        			{
        				t_min = _t < t_min ? _t: t_min;
        			}
        		}
        	}
        }

        return t_min;
    };

private:

};

#endif
