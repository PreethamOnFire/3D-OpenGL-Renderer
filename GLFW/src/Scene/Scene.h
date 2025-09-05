#pragma once
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "Light.h"
#include "Skybox.h"
#include "../Renderer.h"
#include "../Shader.h"
#include "Model/Model.h"

class Scene {
private:
	std::vector<Light> lights;
	std::vector<std::unique_ptr<Model>> models;
	std::unique_ptr<SkyBox> skybox;

	glm::vec3 ambientLight;
	void updateLightUniforms(Shader& shader, glm::vec3& viewPos);
public:
	Scene();
	~Scene();
	void setSkybox(const std::vector<std::string>& faces, std::string& directory, Shader& shader);
	void removeSkybox();
	bool hasSkybox() const;

	Model* addModel(const std::string& name, const std::string& filePath, Shader& shader);
	Model* addCube(const std::string& name, Shader& shader);
	Model* addSphere(const std::string& name, Shader& shader, int segments = 32);
	Model* addPlane(const std::string& name, Shader& shader, float width = 1.0f, float height = 1.0f, unsigned int segments = 1);
	Model* addCustomModel(std::unique_ptr<Model> model);

	bool removeModel(unsigned int id);
	bool removeModel(const std::string& name);

	Model* getModel(unsigned int id);
	Model* getModel(const std::string& name);

	Light* addDirectionalLight(const glm::vec3& dir, const glm::vec3& col, float intensity = 1.0f);
	Light* addPointLight(const glm::vec3& pos, const glm::vec3& col, float intensity = 1.0f);
	Light* addSpotLight(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& col, float innerAngle = 12.5f, float outerAngle = 17.5f, float intensity = 1.0f);
	bool removeLight(size_t index);
	void clearLights();

	void render(Renderer& renderer);


};