// Kyoungduk (Eric) Park
// William Zhang
// April 2019

#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <FL/gl.h>
#include <FL/glu.h>

#define DEFAULT_FOCUS_LENGTH 1.0f
#define NEAR_PLANE 0.01f
#define FAR_PLANE 20.0f
#define VIEW_ANGLE 60.0f
#define PI glm::pi<float>()

class Camera
{

public:

    float rotU, rotV, rotW;

    Camera();
    ~Camera();

    void reset();
    void orientLookAt(glm::vec3 eyePoint, glm::vec3 focusPoint, glm::vec3 upVec);
    void orientLookVec(glm::vec3 eyePoint, glm::vec3 lookVec, glm::vec3 upVec);
    void setViewAngle(float _viewAngle);
    void setNearPlane(float _nearPlane);
    void setFarPlane(float _farPlane);
    void setScreenSize(int _screenWidth, int _screenHeight);
    void setRotUVW(float u, float v, float w);  //called by main.cpp as a part of the slider callback

    glm::mat4 getUnhingeMatrix();
    glm::mat4 getProjectionMatrix();
    glm::mat4 getScalingMatrix();
    glm::mat4 getModelViewMatrix();
    glm::mat4 getInvModelViewMat();
    glm::mat4 getInvTranslationMat();
    glm::mat4 getInvRotationMat();
    glm::mat4 getInvScalingMat();

    void rotateV(float degree);
    void rotateU(float degree);
    void rotateW(float degree);
    void rotate(glm::vec3 point, glm::vec3 axis, float degree);

    void translate(glm::vec3 v);

    glm::vec3 getEyePoint();
    glm::vec3 getLookVector();
    glm::vec3 getUpVector();
    float getViewAngle();
    float getNearPlane();
    float getFarPlane();
    int getScreenWidth();
    int getScreenHeight();

    float getFilmPlanDepth();
    float getScreenWidthRatio();

private:

    glm::vec3 uVec3, vVec3, wVec3;
    glm::vec3 upVec3, lookVec3, eyeVec3;
    glm::mat4 modelViewMat4, projMat4, translationMat4, rotationMat4, scaleMat4, unhingeMat4;
    glm::mat4 rotMatU, rotMatV, rotMatW;

    float viewAngle, filmPlanDepth;
    float nearPlane, farPlane;
    int screenWidth, screenHeight;
    float screenWidthRatio;

    void setScalingMatrix();
    void setRotationMatrix();
    void setTranslationMatrix();
    void setUnhingeMatrix();
    void setModelViewMatrix();
    void setProjectionMatrix();
};

#endif
