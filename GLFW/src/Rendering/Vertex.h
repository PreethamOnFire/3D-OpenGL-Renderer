#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <cmath>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec4 tangent;

    Vertex() : position(0.0f), normal(0.0f, 1.0f, 0.0f), texCoords(0.0f), tangent(1.0f, 0.0f, 0.0f, 1.0f) {}

    Vertex(glm::vec3 pos, glm::vec3 norm = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2 tex = glm::vec2(0.0f),
           glm::vec4 tan = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f))
        : position(pos), normal(norm), texCoords(tex), tangent(tan) {}

    // Convert to flat array format for OpenGL
    static std::vector<float> toFloatArray(const std::vector<Vertex>& vertices) {
        std::vector<float> data;
        data.reserve(vertices.size() * 12); // 3 pos + 3 normal + 2 tex + 4 tangent = 12 floats per vertex

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

            // Tangent + handedness
            data.push_back(vertex.tangent.x);
            data.push_back(vertex.tangent.y);
            data.push_back(vertex.tangent.z);
            data.push_back(vertex.tangent.w);
        }

        return data;
    }

    // Get the vertex layout for OpenGL
    static std::vector<unsigned int> getLayout() {
        return { 3, 3, 2, 4 }; // position, normal, texCoords, tangent(+handedness)
    }

    // Builds the w-signed tangent from a tangent/bitangent pair (e.g. Assimp's mTangents/mBitangents),
    // orthogonalized against the normal. Falls back to an arbitrary perpendicular if the tangent is degenerate.
    static glm::vec4 packTangent(const glm::vec3& n, const glm::vec3& t, const glm::vec3& b) {
        glm::vec3 ortho = t - n * glm::dot(n, t);
        if (glm::dot(ortho, ortho) < 1e-12f) {
            glm::vec3 axis = std::abs(n.x) < 0.9f ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
            ortho = glm::cross(axis, n);
        }
        ortho = glm::normalize(ortho);
        float w = glm::dot(glm::cross(n, ortho), b) < 0.0f ? -1.0f : 1.0f;
        return glm::vec4(ortho, w);
    }

    // Per-triangle UV-derivative tangents accumulated per vertex (for primitives, or meshes Assimp
    // couldn't compute tangents for). Expects normals and texCoords to already be filled in.
    static void computeTangents(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
        std::vector<glm::vec3> tan(vertices.size(), glm::vec3(0.0f));
        std::vector<glm::vec3> bitan(vertices.size(), glm::vec3(0.0f));

        for (size_t i = 0; i + 2 < indices.size(); i += 3) {
            unsigned int i0 = indices[i], i1 = indices[i + 1], i2 = indices[i + 2];
            const Vertex& v0 = vertices[i0];
            const Vertex& v1 = vertices[i1];
            const Vertex& v2 = vertices[i2];

            glm::vec3 e1 = v1.position - v0.position;
            glm::vec3 e2 = v2.position - v0.position;
            glm::vec2 d1 = v1.texCoords - v0.texCoords;
            glm::vec2 d2 = v2.texCoords - v0.texCoords;

            float det = d1.x * d2.y - d2.x * d1.y;
            if (std::abs(det) < 1e-12f) continue; // degenerate UVs -- contributes nothing
            float r = 1.0f / det;

            glm::vec3 t = (e1 * d2.y - e2 * d1.y) * r;
            glm::vec3 b = (e2 * d1.x - e1 * d2.x) * r;

            tan[i0] += t; tan[i1] += t; tan[i2] += t;
            bitan[i0] += b; bitan[i1] += b; bitan[i2] += b;
        }

        for (size_t i = 0; i < vertices.size(); ++i) {
            vertices[i].tangent = packTangent(glm::normalize(vertices[i].normal), tan[i], bitan[i]);
        }
    }
};
