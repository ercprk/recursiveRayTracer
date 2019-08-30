#ifndef CYLINDER_H
#define CYLINDER_H

#include "Shape.h"

class Cylinder : public Shape
{

public:

    Cylinder()
    {
        std::cout << "[Cylinder] Constructor called" << std::endl;
    };

    ~Cylinder()
    {
    	std::cout << "[Cylinder] Destructor called" << std::endl;
    };

    OBJ_TYPE getType()
    {
    	return SHAPE_CYLINDER;
    };

    SceneColor mapTexture(glm::vec3 eye, glm::vec3 ray, double t_, 
        FlatSceneNode* node)
    {   

        glm::mat4 invTransformMat = glm::inverse(node->getTransformMat());
        eye = glm::vec3(invTransformMat * glm::vec4(eye, 1.0));
        ray = glm::vec3(invTransformMat * glm::vec4(ray, 0.0));
        SceneColor texture;

        glm::vec3 point = -(eye + ((float)t_ * ray));
        int s, t;


        if (IN_RANGE(point[Y], .5)) {
            s = (int)(node->textureWidth * node->repeatU * (point[0] + 0.5)) % node->textureWidth;
            t = (int)(node->textureHeight * node->repeatV * (-point[2] + 0.5)) % node->textureHeight;
        }
        else if (IN_RANGE(point[Y], -.5))
        {
            s = (int)(node->textureWidth * node->repeatU * (point[0] + 0.5)) % node->textureWidth;
            t = (int)(node->textureHeight * node->repeatV * (point[2] + 0.5)) % node->textureHeight;
        } else
        {
            double theta = -glm::atan(point[Z], point[X]);

            while (theta < 0)
            {
                theta += 2*PI;
            }

            s = (int)(node->textureWidth * node->repeatU * (theta / (2 * PI))) % node->textureWidth;
            t = (int)(node->textureHeight * node->repeatV * (point[Y] + 0.5)) % node->textureHeight;
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

        glm::vec3 pointObject = eyePointObject + ((float)t * rayObject);
        glm::vec3 normalObject;

        if (IN_RANGE(pointObject[Y], 0.5))
        {
            return glm::vec3(0.0, 1.0, 0.0);
        } 
        else if (IN_RANGE(pointObject[Y], -0.5))
        {
            return glm::vec3(0.0, -1.0, 0.0);
        }
        else
        {
            double theta = glm::atan(pointObject[Z] / pointObject[X]);

            // second quadrant
            if ((pointObject[X] < 0.0 && pointObject[Z] > 0.0 ) || (pointObject[X] < 0.0 && pointObject[Z] < 0.0))
            {
                normalObject = glm::vec3(0.5 * glm::cos(theta + PI), 0.0, 0.5 * glm::sin(theta + PI));
            }
            else if (pointObject[X] > 0.0 && pointObject[Z] < 0.0)
            {
                normalObject = glm::vec3(0.5 * glm::cos(theta + (2.0 * PI)), 0.0, 0.5 * glm::sin(theta + (2.0 * PI)));
            }
            else
            {
                normalObject = glm::vec3(0.5 * glm::cos(theta), 0.0, 0.5 * glm::sin(theta));
            }
            normalObject = glm::normalize(normalObject);

            return normalObject;
        }

        return normalObject;
    };

    double intersect(glm::vec3 eyePointWorld, glm::vec3 rayWorld, glm::mat4 transformMat)
    {
        glm::mat4 invTransformMat = glm::inverse(transformMat);
        glm::vec3 eyePointObject = glm::vec3(invTransformMat * glm::vec4(eyePointWorld, 1.0));
        glm::vec3 rayObject = glm::vec3(invTransformMat * glm::vec4(rayWorld, 0.0));

        // Ray-Body Intersect
        double a = (rayObject[X] * rayObject[X]) + (rayObject[Z] * rayObject[Z]);
        double b = (2.0 * eyePointObject[X] * rayObject[X]) + (2.0 * eyePointObject[Z] * rayObject[Z]);
        double c = (eyePointObject[X] * eyePointObject[X]) + (eyePointObject[Z] * eyePointObject[Z]) - (RADIUS * RADIUS);
        double discriminant = (b * b) - (4.0 * a * c);
        
        // Discriminant Check & Bound Check for Ray-Body Intersect
        double t_body;

        if (discriminant < 0.0)
        {
            t_body = DNE;
        }
        else if (IN_RANGE(discriminant, 0.0))
        {
            t_body = -b / (2.0 * a);
        }
        else
        {
            double t1 = (-b + glm::sqrt(discriminant)) / (2.0 * a);
            double t2 = (-b - glm::sqrt(discriminant)) / (2.0 * a);   
            double y, t_alt;      

            t_body = fmin(t1, t2);
            t_alt = fmin(t1, t2);
            y = eyePointObject[Y] + (t_body * rayObject[Y]);

            if (!within_bounds(y))
            {
                t_body = t_alt;
                y = eyePointObject[Y] + (t_body * rayObject[Y]);

                if (!within_bounds(y))
                {
                    t_body = DNE;
                }
            }
        }

        // Ray-Cap Intersect
        double t_bottom = intersect_plane(eyePointObject, rayObject, glm::vec3(0.0, -0.5, 0.0));
        double t_top = intersect_plane(eyePointObject, rayObject, glm::vec3(0.0, +0.5, 0.0));
        double x_bottom = eyePointObject[X] + (t_bottom * rayObject[X]);
        double z_bottom = eyePointObject[Z] + (t_bottom * rayObject[Z]);
        double x_top = eyePointObject[X] + (t_top * rayObject[X]);
        double z_top = eyePointObject[Z] + (t_top * rayObject[Z]);
        if (!within_circle(x_bottom, z_bottom))
        {
            t_bottom = -1.0;
        }
        if (!within_circle(x_top, z_top))
        {
            t_top = -1.0;
        }

        // Decision between two ray-cap intersects
        double t_cap;

        if ((t_top > 0.0 || IN_RANGE(t_top, 0.0)) && (t_bottom > 0.0 || IN_RANGE(t_bottom, 0.0)))
        {
            t_cap = t_top < t_bottom ? t_top : t_bottom;
        } 
        else if (t_top > 0.0 || IN_RANGE(t_top, 0.0))
        {
            t_cap = t_top;
        }
        else
        {
            t_cap = t_bottom;
        }

        // Decision between Ray-Cap and Ray-Body Intersects
        if ((t_body > 0.0 || IN_RANGE(t_body, 0.0)) && (t_cap > 0.0 || IN_RANGE(t_cap, 0.0)))
        {
            return t_body < t_cap ? t_body : t_cap;
        } 
        else if (t_body > 0.0 || IN_RANGE(t_body, 0.0))
        {
            return t_body;
        }
        else
        {
            return t_cap;
        }
    };

private:    	

};

#endif