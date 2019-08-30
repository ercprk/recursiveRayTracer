#include "FlatSceneTree.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

const float NULL_VECTOR_THRESHOLD = 1e-5; // Check for zero rotation vectors

static glm::mat4 multTransforms(SceneNode* node, glm::mat4 transformMat);

FlatSceneNode::FlatSceneNode()
{
	textureHeight = textureWidth = 0;
	texture = NULL;
	hasTexture = false;
	empty = true;
}

FlatSceneNode::~FlatSceneNode()
{
	int i;

	empty = true;

	for (i = 0; i < textureHeight; i++)
	{
		delete[] texture[i];
	}

	delete[] texture;
}

bool 
FlatSceneNode::isEmpty()
{
	return empty;
}

void 
FlatSceneNode::setPrimitive(ScenePrimitive* _primitive)
{
	assert(_primitive != NULL);
	empty = false;
	primitive = _primitive;
}

void 
FlatSceneNode::setTransformMat(glm::mat4 mat)
{
	empty = false;
	transformMat = mat;
}

ScenePrimitive*
FlatSceneNode::getPrimitive()
{
	assert(primitive != NULL);
	return primitive;
}

glm::mat4
FlatSceneNode::getTransformMat()
{
	return transformMat;
}

void
FlatSceneNode::loadTexture()
{
	SceneFileMap* textureMap;
	ifstream file;
	string line;
	int maxColorValue, i, j, _r, _g, _b;

	textureMap = primitive->material.textureMap;

	if (textureMap->isUsed)
	{
		file.open(textureMap->filename);

		if (!file.is_open())
		{
			std::cerr << "Failed to open file  " << textureMap->filename
				<< std::endl;
			return;
		}

		repeatU = textureMap->repeatU;
		repeatV = textureMap->repeatV;

		file >> line;

		std::getline(file, line);
		std::getline(file, line);

		file >> textureWidth;
		file >> textureHeight;
		file >> maxColorValue;

		texture = new SceneColor*[textureHeight];
		for (i = 0; i < textureHeight; i++)
		{
			texture[i] = new SceneColor[textureWidth];
		}

		for (i = 0; i < textureHeight; i++)
		{
			for (j = 0; j < textureWidth; j++)
			{
				file >> _r;
				file >> _g;
				file >> _b;

				texture[i][j].r = (float)_r / (float)maxColorValue;
				texture[i][j].g = (float)_g / (float)maxColorValue;
				texture[i][j].b = (float)_b / (float)maxColorValue;
			}
		}

		hasTexture = true;

		file.close();
		std::cout << "[ FlatSceneNode] Texture Loaded" << std::endl;
	}
}

FlatSceneTree::FlatSceneTree()
{
	flatTree.clear();
	std::cout << "Flat Tree Constructed" << std::endl;
}

FlatSceneTree::~FlatSceneTree()
{
	clear();
	std::cout << "Flat Tree Destructed" << std::endl;
}

bool
FlatSceneTree::isEmpty()
{
	return flatTree.empty();
}

int
FlatSceneTree::getSize()
{
	return flatTree.size();
}

void
FlatSceneTree::clear()
{
	int i, num_nodes;

	num_nodes = flatTree.size();

	for (i = 0; i < num_nodes; i++)
	{
		delete flatTree[i];
	}

	std::cout << "Flat Tree Deallocated" << std::endl;
	flatTree.clear();
}

void 
FlatSceneTree::flattenSceneTree(SceneNode* rootNode)
{
	assert(rootNode != NULL);
	flattenSceneNode(rootNode, glm::mat4(1.0f));
}

FlatSceneNode* 
FlatSceneTree::getNode(int index)
{
	FlatSceneNode* node = flatTree[index];
	assert(node != NULL);
	return node;
}

void
FlatSceneTree::flattenSceneNode(SceneNode* node, glm::mat4 transformMat)
{
	int i, num_primitives, num_children;

	transformMat = multTransforms(node, transformMat);
	 
	if (!(node->primitives.empty()))
	{
		num_primitives = node->primitives.size();

		for (i = 0; i < num_primitives; i++)
		{
			FlatSceneNode* newNode = new FlatSceneNode();
			newNode->setPrimitive(node->primitives[i]);
			newNode->setTransformMat(transformMat);
			newNode->loadTexture();
			flatTree.push_back(newNode);
		}
	}

	if (!(node->children.empty()))
	{
		num_children = node->children.size();

		for (i = 0; i < num_children; i++)
		{
			flattenSceneNode(node->children[i], transformMat);
		}
	}
}

static glm::mat4 
multTransforms(SceneNode* node, glm::mat4 transformMat)
{
	int i, num_transforms; 
	int type_transform; // Type of transformation; look at SceneData.h
	glm::mat4 mat; // Transformation matrix at this node, if exists

	if (!(node->transformations.empty()))
	{
		num_transforms = node->transformations.size();

		for (i = 0; i < num_transforms; i++)
		{
			type_transform = node->transformations[i]->type;

			switch (type_transform)
			{
				case TRANSFORMATION_TRANSLATE:
					mat = glm::translate(node->transformations[i]->translate);
					break;
				case TRANSFORMATION_ROTATE:
					mat = glm::length(node->transformations[i]->rotate) 
						> NULL_VECTOR_THRESHOLD 
						? glm::rotate(node->transformations[i]->angle, 
						node->transformations[i]->rotate)
						: glm::mat4(1.0f);
					break;
				case TRANSFORMATION_SCALE:
					mat = glm::scale(node->transformations[i]->scale);
					break;
				case TRANSFORMATION_MATRIX:
					mat = node->transformations[i]->matrix;
					break;
				default:
					std::cerr << "Fucked Up Transformation Type" << std::endl;
					exit(1);
					break;
			}

			transformMat = transformMat * mat;
		}
	}

	return transformMat;
}