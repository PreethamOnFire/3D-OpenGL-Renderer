#include <string>
#include <vector>
#include "../Rendering/Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ModelLoader.h"
#include <assimp/mesh.h>
#include "../Rendering/Vertex.h"
#include "../Loaders/TextureLoader.h"
#include <filesystem>
#include <iostream>

std::unique_ptr<SceneNode> ModelLoader::loadHierarchicalModel(const std::string& filePath, ShaderPipeline& pipeline, MaterialLibrary& materials, const std::string& modelName) {
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
        return nullptr;
    }

    std::string directory = std::filesystem::path(filePath).parent_path().string();

	loadMaterials(scene, directory, pipeline, materials, modelName);
	return processNode(scene->mRootNode, scene, modelName);
}

std::unique_ptr<Mesh> ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene, const std::string& modelName) {
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

    std::string materialName = modelName + "_mat_" + std::to_string(mesh->mMaterialIndex);
    return std::make_unique<Mesh>(vertices, indices, materialName);
}

std::unique_ptr<SceneNode> ModelLoader::processNode(aiNode* node, const aiScene* scene, const std::string& modelName) {
    auto sceneNode = std::make_unique<SceneNode>(node->mName.C_Str());

	aiMatrix4x4 aiTransform = node->mTransformation;
	aiVector3D scaling, position;
	aiQuaternion rotation;

    aiTransform.Decompose(scaling, rotation, position);
	sceneNode->setPosition(glm::vec3(position.x, position.y, position.z));
    sceneNode->setScale(glm::vec3(scaling.x, scaling.y, scaling.z));
	sceneNode->setRotation(quatToEuler(rotation));

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* childMesh = scene->mMeshes[node->mMeshes[i]];
        auto mesh = processMesh(childMesh, scene, modelName);
        if (mesh) {
            sceneNode->addMesh(std::move(mesh));
        }
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
		auto childNode = processNode(node->mChildren[i], scene, modelName);

        if (childNode) {
            sceneNode->addChild(std::move(childNode));
        }
    }
	return sceneNode;
}

void ModelLoader::loadMaterials(const aiScene* scene, const std::string& directory, ShaderPipeline& pipeline, MaterialLibrary& materials, const std::string& modelName) {
    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial* aiMat = scene->mMaterials[i];
        std::string materialName = modelName + "_mat_" + std::to_string(i);
        Material& material = materials.create(materialName, pipeline);

		aiColor3D color(0.0f, 0.0f, 0.0f);
		glm::vec3 diffuse(0.8f, 0.8f, 0.8f);
		glm::vec3 ambient = diffuse * 0.2f;
		glm::vec3 specular(1.0f, 1.0f, 1.0f);
		float shininess = 32.0f;

        if (AI_SUCCESS == aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
            diffuse = glm::vec3(color.r, color.g, color.b);
        }
        if (AI_SUCCESS == aiMat->Get(AI_MATKEY_COLOR_AMBIENT, color)) {
            ambient = glm::vec3(color.r, color.g, color.b);
        }
        else {
			ambient = diffuse * 0.2f;
        }
        if (AI_SUCCESS == aiMat->Get(AI_MATKEY_COLOR_SPECULAR, color)) {
            specular = glm::vec3(color.r, color.g, color.b);
		}
        float aiShininess = 0.0f;
        if (AI_SUCCESS == aiMat->Get(AI_MATKEY_SHININESS, aiShininess)) {
            shininess = aiShininess;
        }

        material.setVec3("diffuse", diffuse);
        material.setVec3("ambient", ambient);
        material.setVec3("specular", specular);
        material.setFloat("shininess", shininess);

        std::vector<Texture> diffuseMaps = TextureLoader::loadMaterialTextures(aiMat, aiTextureType_DIFFUSE, "diffuse", directory);
        for (auto& texture : diffuseMaps) {
            material.setTexture("diffuse", texture);
        }

		std::vector<Texture> specularMaps = TextureLoader::loadMaterialTextures(aiMat, aiTextureType_SPECULAR, "specular", directory);
        for (auto& texture : specularMaps) {
            material.setTexture("specular", texture);
        }

		std::vector<Texture> normalMaps = TextureLoader::loadMaterialTextures(aiMat, aiTextureType_HEIGHT, "normal", directory);
        for (auto& texture : normalMaps) {
            material.setTexture("normal", texture);
        }
    }

    if (scene->mNumMaterials == 0) {
        std::string materialName = modelName + "_mat_0";
        Material& material = materials.create(materialName, pipeline);
        material.setVec3("diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        std::cout << "Created default material" << std::endl;
    }
}

glm::vec3 ModelLoader::quatToEuler(const aiQuaternion& q) {
    float x = atan2(2.0f * (q.w * q.x + q.y * q.z), 1.0f - 2.0f * (q.x * q.x + q.y * q.y));
    float y = asin(std::max(-1.0f, std::min(1.0f, 2.0f * (q.w * q.y - q.z * q.x))));
    float z = atan2(2.0f * (q.w * q.z + q.x * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z));
    return glm::vec3(glm::degrees(x), glm::degrees(y), glm::degrees(z));
}
