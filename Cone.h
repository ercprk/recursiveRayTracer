// Kyoungduk (Eric) Park
// William Zhang
// April 2019

#ifndef CONE_H
#define CONE_H

#include "Shape.h"

class Cone : public Shape
{

public:

    Cone()
    {
        std::cout << "[Cone] Constructor called" << std::endl;
    };

    ~Cone()
    {
        std::cout << "[Cone] Destructor called" << std::endl;
    };

    OBJ_TYPE getType()
    {
        return SHAPE_CONE;
    };

    SceneColor mapTexture(glm::vec3 eye, glm::vec3 ray, double t_,
                          FlatSceneNode * node)
    {
        glm::mat4 invTransformMat = glm::inverse(node->getTransformMat());
        eye = glm::vec3(invTransformMat * glm::vec4(eye, 1.0));
        ray = glm::vec3(invTransformMat * glm::vec4(ray, 0.0));
        SceneColor texture;

        glm::vec3 point = -(eye + ((float)t_ * ray));
        int s, t;

        if (IN_RANGE(point[Y], -0.5))
        {
            s = (int)(node->textureWidth * node->repeatU * (point[X] + 0.5)) % node->textureWidth;
            t = (int)(node->textureHeight * node->repeatV * (-point[Z] + 0.5)) % node->textureHeight;
        }
        else
        {
            double theta = -glm::atan(point[Z], point[X]);

            while (theta < 0.0)
            {
                theta += 2 * PI;
            }

            s = (int)(node->textureWidth * node->repeatU * (theta / (2 * PI))) % node->textureWidth;
            t = (int)(node->textureHeight * node->repeatV * (point[Y] + 0.5)) % node->textureHeight;
        }

        texture.r = node->texture[t][s].r;
        texture.g = node->texture[t][s].g;
        texture.b = node->texture[t][s].b;

        //std::cout << "here" << std::endl;
        //std::cout << texture.r << " " << texture.g << " " << texture.b << std::endl;

        return texture;
    };

    glm::vec3 intersect_normal(glm::vec3 eyePointWorld, glm::vec3 rayWorld, glm::mat4 transformMat, double t)
    {
        glm::mat4 invTransformMat = glm::inverse(transformMat);
        glm::vec3 eyePointObject = glm::vec3(invTransformMat * glm::vec4(eyePointWorld, 1.0));
        glm::vec3 rayObject = glm::vec3(invTransformMat * glm::vec4(rayWorld, 0.0));

        glm::vec3 pointObject = eyePointObject + ((float)t * rayObject);

        if (IN_RANGE(pointObject[Y], -0.5))
        {
            return glm::vec3(0.0, -1.0, 0.0);
        }
        else
        {
            double x = pointObject[X];
            double z = pointObject[Z];

            return glm::normalize(glm::normalize(glm::vec3(x, 0.0, z)) + glm::vec3(0.0, 0.5, 0.0));
        }
    };

    double intersect(glm::vec3 eyePointWorld, glm::vec3 rayWorld, glm::mat4 transformMat)
    {
        glm::mat4 invTransformMat = glm::inverse(transformMat);
        glm::vec3 eyePointObject = glm::vec3(invTransformMat * glm::vec4(eyePointWorld, 1.0));
        glm::vec3 rayObject = glm::vec3(invTransformMat * glm::vec4(rayWorld, 0.0));

        // Constant for radius / height
        double k = (RADIUS / (OFFSET + OFFSET));
        // Constant for apex - eyePoint_y_coordinate
        double yk = OFFSET - eyePointObject[Y];

        // Ray-Body Intersect
        double a = (rayObject[X] * rayObject[X]) + (rayObject[Z] * rayObject[Z]) - (k * k * rayObject[Y] * rayObject[Y]);
        double b = (2.0 * eyePointObject[X] * rayObject[X]) + (2.0 * eyePointObject[Z] * rayObject[Z]) + (2.0 * k * k * yk * rayObject[Y]);
        double c = (eyePointObject[X] * eyePointObject[X]) + (eyePointObject[Z] * eyePointObject[Z]) - (k * k * yk * yk);
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
            t_alt = fmax(t1, t2);
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
        double t_cap = intersect_plane(eyePointObject, rayObject, glm::vec3(0.0, -0.5, 0.0));
        double x = eyePointObject[X] + (t_cap * rayObject[X]);
        double z = eyePointObject[Z] + (t_cap * rayObject[Z]);
        if (!within_circle(x, z)) // checks within the cap bound of radius 0.5
        {
            t_cap = -1.0;
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
