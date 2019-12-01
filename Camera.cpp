// Kyoungduk (Eric) Park
// William Zhang
// April 2019

#include "Camera.h"
#include <FL/gl.h>
#include <FL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Camera::Camera()
{
    std::cout << "[Camera] Constructor called" << std::endl;
    reset();
}

Camera::~Camera()
{
    std::cout << "[Camera] Destructor called" << std::endl;
}

void Camera::reset()
{
    orientLookAt(glm::vec3(0.0f, 0.0f, DEFAULT_FOCUS_LENGTH), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    setViewAngle(VIEW_ANGLE);
    setNearPlane(NEAR_PLANE);
    setFarPlane(FAR_PLANE);
    screenWidth = screenHeight = 200;
    screenWidthRatio = 1.0f;
    rotU = rotV = rotW = 0;
    rotateU(0.0);
    rotateV(0.0);
    rotateW(0.0);
}

//called by main.cpp as a part of the slider callback for controlling rotation
//the reason for computing the diff is to make sure that we are only incrementally rotating the camera
void Camera::setRotUVW(float u, float v, float w)
{
    rotateU(u);
    rotateV(v);
    rotateW(w);
    setModelViewMatrix();
    rotU = u;
    rotV = v;
    rotW = w;
}

void Camera::orientLookAt(glm::vec3 eyePoint, glm::vec3 lookatPoint, glm::vec3 upVec)
{
    upVec3 = upVec;
    eyeVec3 = eyePoint;
    lookVec3 = glm::vec3(lookatPoint[0] - eyePoint[0], lookatPoint[1] - eyePoint[1], lookatPoint[2] - eyePoint[2]);

    wVec3 = glm::vec3(-1.0 * lookVec3[0], -1.0 * lookVec3[1], -1.0 * lookVec3[2]);
    wVec3 = glm::normalize(wVec3);
    uVec3 = glm::cross(upVec3, wVec3);
    uVec3 = glm::normalize(uVec3);
    vVec3 = glm::cross(wVec3, uVec3);
    vVec3 = glm::normalize(vVec3);

    setTranslationMatrix();
    setRotationMatrix();
}


void Camera::orientLookVec(glm::vec3 eyePoint, glm::vec3 lookVec, glm::vec3 upVec)
{
    lookVec3 = lookVec;
    upVec3 = upVec;
    eyeVec3 = eyePoint;

    wVec3 = glm::vec3(-1.0 * lookVec3[0], -1.0 * lookVec3[1], -1.0 * lookVec3[2]);
    wVec3 = glm::normalize(wVec3);
    uVec3 = glm::cross(upVec3, wVec3);
    uVec3 = glm::normalize(uVec3);
    vVec3 = glm::cross(wVec3, uVec3);
    vVec3 = glm::normalize(vVec3);

    setTranslationMatrix();
    setRotationMatrix();
}

void Camera::setScalingMatrix()
{
    float aspectRatio = (float)screenHeight / (float)screenWidth;
    float theta_w = glm::radians((float)viewAngle);
    float theta_h = theta_w * aspectRatio;

    glm::vec4 col0 = glm::vec4(1.0 / (glm::tan(theta_w / 2.0) * (float)farPlane), 0.0, 0.0, 0.0);
    glm::vec4 col1 = glm::vec4(0.0, 1.0 / (glm::tan(theta_w / 2.0) * (float)farPlane * aspectRatio), 0.0, 0.0);
    glm::vec4 col2 = glm::vec4(0.0, 0.0, 1.0 / (float)farPlane, 0.0);
    glm::vec4 col3 = glm::vec4(0.0, 0.0, 0.0, 1.0);

    scaleMat4 = glm::mat4(col0, col1, col2, col3);

    setProjectionMatrix();
}

glm::mat4 Camera::getInvRotationMat()
{
    return glm::transpose(rotationMat4);
}

void Camera::setUnhingeMatrix()
{
    float c = -((float)nearPlane / (float)farPlane);

    glm::vec4 col0 = glm::vec4(1.0, 0.0, 0.0, 0.0);
    glm::vec4 col1 = glm::vec4(0.0, 1.0, 0.0, 0.0);
    glm::vec4 col2 = glm::vec4(0.0, 0.0, -(1.0 / (c + 1.0)), -1.0);
    glm::vec4 col3 = glm::vec4(0.0, 0.0, c / (c + 1.0), 0.0);

    unhingeMat4 = glm::mat4(col0, col1, col2, col3);

    setProjectionMatrix();
}

void Camera::setProjectionMatrix()
{
    projMat4 = unhingeMat4 * scaleMat4;
}

glm::mat4 Camera::getScalingMatrix()
{
    return scaleMat4;
}

glm::mat4 Camera::getInvScalingMat()
{
    return glm::inverse(scaleMat4);
}

glm::mat4 Camera::getUnhingeMatrix()
{
    return unhingeMat4;
}

glm::mat4 Camera::getProjectionMatrix()
{
    return projMat4;
}

void Camera::setViewAngle (float _viewAngle)
{
    viewAngle = _viewAngle;
    setScalingMatrix();
}

void Camera::setNearPlane (float _nearPlane)
{
    nearPlane = _nearPlane;
    setUnhingeMatrix();
}

void Camera::setFarPlane (float _farPlane)
{
    farPlane = _farPlane;
    setScalingMatrix();
    setUnhingeMatrix();
}

void Camera::setScreenSize (int _screenWidth, int _screenHeight)
{
    screenWidth = _screenWidth;
    screenHeight = _screenHeight;
    setScalingMatrix();
}

void Camera::setModelViewMatrix()
{
    modelViewMat4 = rotMatW * rotMatV * rotMatU * rotationMat4 * translationMat4;
}

void Camera::setTranslationMatrix()
{
    glm::vec4 col0 = glm::vec4(1.0, 0.0, 0.0, 0.0);
    glm::vec4 col1 = glm::vec4(0.0, 1.0, 0.0, 0.0);
    glm::vec4 col2 = glm::vec4(0.0, 0.0, 1.0, 0.0);
    glm::vec4 col3 = glm::vec4(-eyeVec3[0], -eyeVec3[1], -eyeVec3[2], 1.0);
    translationMat4 = glm::mat4(col0, col1, col2, col3);
    setModelViewMatrix();
}

void Camera::setRotationMatrix()
{
    glm::vec4 col0 = glm::vec4(uVec3[0], vVec3[0], wVec3[0], 0.0);
    glm::vec4 col1 = glm::vec4(uVec3[1], vVec3[1], wVec3[1], 0.0);
    glm::vec4 col2 = glm::vec4(uVec3[2], vVec3[2], wVec3[2], 0.0);
    glm::vec4 col3 = glm::vec4(0.0, 0.0, 0.0, 1.0);
    rotationMat4 = glm::mat4(col0, col1, col2, col3);
    setModelViewMatrix();
}


glm::mat4 Camera::getInvTranslationMat()
{
    glm::vec4 col0 = glm::vec4(1.0, 0.0, 0.0, 0.0);
    glm::vec4 col1 = glm::vec4(0.0, 1.0, 0.0, 0.0);
    glm::vec4 col2 = glm::vec4(0.0, 0.0, 1.0, 0.0);
    glm::vec4 col3 = glm::vec4(eyeVec3[0], eyeVec3[1], eyeVec3[2], -1.0);
    return glm::mat4(col0, col1, col2, col3);
}

glm::mat4 Camera::getModelViewMatrix()
{
    return modelViewMat4;
}

glm::mat4 Camera::getInvModelViewMat()
{
    return glm::inverse(modelViewMat4);
}

void Camera::rotateU(float degrees)
{
    float radians = glm::radians(degrees);
    glm::vec4 col0 = glm::vec4(1.0, 0.0, 0.0, 0.0);
    glm::vec4 col1 = glm::vec4(0.0, glm::cos(radians), glm::sin(radians), 0.0);
    glm::vec4 col2 = glm::vec4(0.0f, -glm::sin(radians), glm::cos(radians), 0.0);
    glm::vec4 col3 = glm::vec4(0.0, 0.0, 0.0, 1.0);
    rotMatU = glm::mat4(col0, col1, col2, col3);
}

void Camera::rotateV(float degrees)
{
    float radians = glm::radians(degrees);
    glm::vec4 col0 = glm::vec4(glm::cos(radians), 0.0, -glm::sin(radians), 0.0);
    glm::vec4 col1 = glm::vec4(0.0, 1.0, 0.0, 0.0);
    glm::vec4 col2 = glm::vec4(glm::sin(radians), 0.0, glm::cos(radians), 0.0);
    glm::vec4 col3 = glm::vec4(0.0, 0.0, 0.0, 1.0);
    rotMatV = glm::mat4(col0, col1, col2, col3);
}

void Camera::rotateW(float degrees)
{
    float radians = glm::radians(degrees);
    glm::vec4 col0 = glm::vec4(glm::cos(radians), glm::sin(radians), 0.0, 0.0);
    glm::vec4 col1 = glm::vec4(-glm::sin(radians), glm::cos(radians), 0.0, 0.0);
    glm::vec4 col2 = glm::vec4(0.0, 0.0, 1.0, 0.0);
    glm::vec4 col3 = glm::vec4(0.0, 0.0, 0.0, 1.0);
    rotMatW = glm::mat4(col0, col1, col2, col3);
}

void Camera::translate(glm::vec3 v)
{
    glm::vec4 col0 = glm::vec4(1.0, 0.0, 0.0, 0.0);
    glm::vec4 col1 = glm::vec4(0.0, 1.0, 0.0, 0.0);
    glm::vec4 col2 = glm::vec4(0.0, 0.0, 1.0, 0.0);
    glm::vec4 col3 = glm::vec4(-v[0], -v[1], -v[2], 1.0);
    glm::mat4 _translationMat4 = glm::mat4(col0, col1, col2, col3);

    glm::vec4 eyeVec4 = _translationMat4 * glm::vec4(eyeVec3, 1.0);
    glm::vec4 lookVec4 = _translationMat4 * glm::vec4(eyeVec3, 1.0);
    glm::vec4 upVec4 = _translationMat4 * glm::vec4(eyeVec3, 1.0);

    eyeVec3 = glm::vec3(eyeVec4[0], eyeVec4[1], eyeVec4[2]);
    lookVec3 = glm::vec3(lookVec4[0], lookVec4[1], lookVec4[2]);
    upVec3 = glm::vec3(upVec4[0], upVec4[1], upVec4[2]);
}

void Camera::rotate(glm::vec3 point, glm::vec3 axis, float degrees)
{
    float m1theta = -glm::atan(axis[0] / axis[2]);
    float d = glm::sqrt(glm::pow(axis[0], 2) + glm::pow(axis[1], 2));
    float m2theta = glm::asin(axis[0] / d) + PI / 2.0;
    float m3theta = glm::radians(degrees);

    glm::vec4 m1col0 = glm::vec4(glm::cos(m1theta), 0.0, -glm::sin(m1theta), 0.0);
    glm::vec4 m1col1 = glm::vec4(0.0, 1.0, 0.0, 0.0);
    glm::vec4 m1col2 = glm::vec4(glm::sin(m1theta), 0.0, glm::cos(m1theta), 0.0);
    glm::vec4 m1col3 = glm::vec4(0.0, 0.0, 0.0, 1.0);
    glm::vec4 m2col0 = glm::vec4(glm::cos(m2theta), glm::sin(m2theta), 0.0, 0.0);
    glm::vec4 m2col1 = glm::vec4(-glm::sin(m2theta), glm::cos(m2theta), 0.0, 0.0);
    glm::vec4 m2col2 = glm::vec4(0.0, 0.0, 1.0, 0.0);
    glm::vec4 m2col3 = glm::vec4(0.0, 0.0, 0.0, 1.0);
    glm::vec4 m3col0 = glm::vec4(1.0, 0.0, 0.0, 0.0);
    glm::vec4 m3col1 = glm::vec4(0.0, glm::cos(m3theta), glm::sin(m3theta), 0.0);
    glm::vec4 m3col2 = glm::vec4(0.0, -glm::sin(m3theta), glm::cos(m3theta), 0.0);
    glm::vec4 m3col3 = glm::vec4(0.0, 0.0, 0.0, 1.0);

    glm::mat4 m1 = glm::mat4(m1col0, m1col1, m1col2, m1col3);
    glm::mat4 m2 = glm::mat4(m2col0, m2col1, m2col2, m2col3);
    glm::mat4 m3 = glm::mat4(m3col0, m3col1, m3col2, m3col3);

    glm::vec4 result = m1 * m2 * m3 * glm::vec4(point, 1.0);
}

glm::vec3 Camera::getEyePoint()
{
    return eyeVec3;
}

glm::vec3 Camera::getLookVector()
{
    return lookVec3;
}

glm::vec3 Camera::getUpVector()
{
    return upVec3;
}

float Camera::getViewAngle()
{
    return viewAngle;
}

float Camera::getNearPlane()
{
    return nearPlane;
}

float Camera::getFarPlane()
{
    return farPlane;
}

int Camera::getScreenWidth()
{
    return screenWidth;
}

int Camera::getScreenHeight()
{
    return screenHeight;
}

float Camera::getFilmPlanDepth()
{
    return filmPlanDepth;
}

float Camera::getScreenWidthRatio()
{
    return (float)screenWidth / (float)screenHeight;
}
