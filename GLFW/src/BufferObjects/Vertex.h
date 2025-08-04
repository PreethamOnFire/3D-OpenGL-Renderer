#pragma once
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;

    Vertex() : position(0.0f), normal(0.0f, 1.0f, 0.0f), texCoords(0.0f) {}

    Vertex(glm::vec3 pos, glm::vec3 norm = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2 tex = glm::vec2(0.0f))
        : position(pos), normal(norm), texCoords(tex) {}

    // Convert to flat array format for OpenGL
    static std::vector<float> toFloatArray(const std::vector<Vertex>& vertices) {
        std::vector<float> data;
        data.reserve(vertices.size() * 8); // 3 pos + 3 normal + 2 tex = 8 floats per vertex

        for (const auto& vertex : vertices) {
            // Position
            data.push_back(vertex.position.x);
            data.push_back(vertex.position.y);
            data.push_back(vertex.position.z);

            // Normal
            data.push_back(vertex.normal.x);
            data.push_back(vertex.normal.y);
            data.push_back(vertex.normal.z);

            // Texture coordinates
            data.push_back(vertex.texCoords.x);
            data.push_back(vertex.texCoords.y);
        }

        return data;
    }

    // Get the vertex layout for OpenGL
    static std::vector<unsigned int> getLayout() {
        return { 3, 3, 2 }; // 3 floats for position, 3 for normal, 2 for texCoords
    }
};