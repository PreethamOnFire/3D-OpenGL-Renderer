#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../Mesh.h"
#include "../../Shader.h"

class Cube {
private:
	std::vector<Vertex> createCubeVertices();
	std::vector<unsigned int> createCubeIndices();
public:
	Mesh cubeMesh;
    Shader* shader;
	Cube(Shader &shader);
	~Cube();
};