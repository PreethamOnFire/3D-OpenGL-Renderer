#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../Mesh.h"
#include "../../Shader.h"

class Cube {
private:
    static constexpr int POSITION_COUNT = 48;
    static constexpr int INDEX_COUNT = 36;
    std::vector<unsigned int> layout = { 3, 3 };
    float positions[POSITION_COUNT] = {
        // Front face
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, // Bottom-left (red)
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // Bottom-right (green)
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, // Top-right (blue)
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, // Top-left (yellow)

        // Back face
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f, // Bottom-left (magenta)
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, // Bottom-right (cyan)
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, // Top-right (white)
        -0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f  // Top-left (gray)
    };

    unsigned int indices[INDEX_COUNT] = {
        // Front face
        0, 1, 2,   2, 3, 0,
        // Back face
        4, 5, 6,   6, 7, 4,
        // Left face
        7, 3, 0,   0, 4, 7,
        // Right face
        1, 5, 6,   6, 2, 1,
        // Top face
        3, 2, 6,   6, 7, 3,
        // Bottom face
        0, 1, 5,   5, 4, 0
    };

public:
	Mesh cubeMesh;
    Shader* shader;
	Cube(Shader &shader);
	~Cube();
};