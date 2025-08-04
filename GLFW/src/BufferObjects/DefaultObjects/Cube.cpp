#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Cube.h"

Cube::Cube(Shader &shader) : cubeMesh(positions, sizeof(positions), indices, sizeof(indices),
	shader, layout, 36) {
	this->shader = &shader;
}

Cube::~Cube() {
}