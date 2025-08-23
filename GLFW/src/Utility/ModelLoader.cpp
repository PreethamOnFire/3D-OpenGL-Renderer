#include <string>
#include <vector>
#include "../BufferObjects/Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ModelLoader.h"
#include <assimp/mesh.h>
#include "../BufferObjects/Vertex.h"

Mesh* ModelLoader::loadModel(const std::string& filePath, Shader& shader) {
    auto result = loadHierarchicalModel(filePath, shader);
    if (result.first && !result.first->meshes.empty()) {
        // Return first mesh found in hierarchy
        return result.first->meshes[0];
    }
    return nullptr;
}

std::pair<std::unique_ptr<SceneNode>, std::vector<Material*>> ModelLoader::loadHierarchicalModel(const std::string& filePath, Shader& shader) {
	std::vector<Material*> materials;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath,
        aiProcess_Triangulate |           // Convert polygons to triangles
        aiProcess_FlipUVs |              // Flip Y coordinate of UV (OpenGL convention)
        aiProcess_GenNormals |           // Generate normals if missing
        aiProcess_CalcTangentSpace |     // Calculate tangents for normal mapping
        aiProcess_JoinIdenticalVertices | // Optimize duplicate vertices
        aiProcess_SortByPType |          // Sort by primitive type
        aiProcess_RemoveRedundantMaterials | // Remove redundant materials
        aiProcess_OptimizeMeshes         // Merge small meshes
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return { nullptr, materials };
    }

	materials = loadMaterials(scene);
	auto rootNode = processNode(scene->mRootNode, scene, shader);
	return { std::move(rootNode), materials };
}

Mesh* ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene, Shader& shader) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        if (mesh->HasNormals()) {
            vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }
        else {
            vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f); // Default normal up
        }

        if (mesh->mTextureCoords[0]) {
            vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        } else {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
	}

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }
    Mesh* processedMesh = new Mesh(vertices, indices, shader);
	return processedMesh;
}

std::unique_ptr<SceneNode> ModelLoader::processNode(aiNode* node, const aiScene* scene, Shader& shader) {
    auto sceneNode = std::make_unique<SceneNode>(node->mName.C_Str());

	aiMatrix4x4 aiTransform = node->mTransformation;
	aiVector3D scaling, position;
	aiQuaternion rotation;

    aiTransform.Decompose(scaling, rotation, position);
	sceneNode->setPosition(glm::vec3(position.x, position.y, position.z));
    sceneNode->setScale(glm::vec3(scaling.x, scaling.y, scaling.z));
	sceneNode->setRotation(quatToEuler(rotation));

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        aiMesh* childMesh = scene->mMeshes[node->mMeshes[i]];
        Mesh* mesh = processMesh(childMesh, scene, shader);

        if (mesh) {
            sceneNode->meshes.push_back(mesh);
            sceneNode->materialIndices.push_back(childMesh->mMaterialIndex);
        }
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
		auto childNode = processNode(node->mChildren[i], scene, shader);

        if (childNode) {
            sceneNode->addChild(std::move(childNode));
        }
    }
	return sceneNode;
}

std::vector<Material*> ModelLoader::loadMaterials(const aiScene* scene) {
    std::vector<Material*> materials;
    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial* aiMat = scene->mMaterials[i];
        Material* material = new Material();

		aiColor3D color(0.0f, 0.0f, 0.0f);

        if (AI_SUCCESS == aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
            material->diffuse = glm::vec3(color.r, color.g, color.b);
        }
        if (AI_SUCCESS == aiMat->Get(AI_MATKEY_COLOR_AMBIENT, color)) {
            material->ambient = glm::vec3(color.r, color.g, color.b);
        }
        else {
			material->ambient = material->diffuse * 0.2f; 
        }
        if (AI_SUCCESS == aiMat->Get(AI_MATKEY_COLOR_SPECULAR, color)) {
            material->specular = glm::vec3(color.r, color.g, color.b);
		}
        float shininess = 0.0f;
        if (AI_SUCCESS == aiMat->Get(AI_MATKEY_SHININESS, shininess)) {
            material->shininess = shininess;
        }
		materials.push_back(material);
    }

    // if no materials exist make one
    if (materials.empty()) {
        Material* defaultMaterial = new Material();
        defaultMaterial->diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
        materials.push_back(defaultMaterial);
        std::cout << "Created default material" << std::endl;
    }

    return materials;
}

glm::vec3 ModelLoader::quatToEuler(const aiQuaternion& q) {
    float x = atan2(2.0f * (q.w * q.x + q.y * q.z), 1.0f - 2.0f * (q.x * q.x + q.y * q.y));
    float y = asin(std::max(-1.0f, std::min(1.0f, 2.0f * (q.w * q.y - q.z * q.x))));
    float z = atan2(2.0f * (q.w * q.z + q.x * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z));
    return glm::vec3(glm::degrees(x), glm::degrees(y), glm::degrees(z));
}
