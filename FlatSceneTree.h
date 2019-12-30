// Project : Recursive Ray Tracer
// File    : FlatSceneTree.h
// Authors : Kyoungduk (Eric) Park
//           William Zhang

#ifndef FLAT_SCENE_TREE_H
#define FLAT_SCENE_TREE_H

#include "./scene/SceneData.h"
// #include "./scene/SceneParser.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <vector>
#include <glm/glm.hpp>

using namespace std;

class FlatSceneNode
{

public:

    FlatSceneNode();
    ~FlatSceneNode();
    void setPrimitive(ScenePrimitive* _primitive);
    void setTransformMat(glm::mat4 mat);
    ScenePrimitive* getPrimitive();
    glm::mat4 getTransformMat();
    bool isEmpty();

    SceneColor** texture;
    int textureWidth, textureHeight;
    bool hasTexture;
    int repeatU, repeatV;
    void loadTexture();

private:

    ScenePrimitive* primitive;
    glm::mat4 transformMat;
    bool empty;
};

class FlatSceneTree
{

public:

    FlatSceneTree();
    ~FlatSceneTree();
    void flattenSceneTree(SceneNode* rootNode);
    bool isEmpty();
    int getSize();
    void clear();
    FlatSceneNode* getNode(int index);

private:

    void flattenSceneNode(SceneNode* node, glm::mat4 transformMat);
    std::vector<FlatSceneNode*> flatTree;

};

#endif
