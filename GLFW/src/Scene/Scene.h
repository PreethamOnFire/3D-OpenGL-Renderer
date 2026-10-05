#pragma once
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "Light.h"
#include "SkyBox.h"
#include "../Core/Renderer.h"
#include "../Core/ShaderPipeline.h"
#include "../Rendering/MaterialLibrary.h"
#include "../Rendering/RenderQueue.h"
#include "Model.h"
#include "SceneStats.h"

class Scene {
private:
	std::vector<Light> lights;
	std::vector<std::unique_ptr<Model>> models;
	std::unique_ptr<SkyBox> skybox;
	RenderQueue renderQueue;

	glm::vec3 ambientLight;
	void updateLightUniforms(ShaderPipeline& pipeline, const glm::vec3& viewPos);
public:
	Scene();
	~Scene();
	void setSkybox(const std::vector<std::string>& faces, const std::string& directory, ShaderPipeline& pipeline);
	void removeSkybox();
	bool hasSkybox() const;

	Model* addModel(const std::string& name, const std::string& filePath, ShaderPipeline& pipeline, MaterialLibrary& materials);
	Model* addCube(const std::string& name, ShaderPipeline& pipeline, MaterialLibrary& materials);
	Model* addSphere(const std::string& name, ShaderPipeline& pipeline, MaterialLibrary& materials, int segments = 32);
	Model* addPlane(const std::string& name, ShaderPipeline& pipeline, MaterialLibrary& materials, float width = 1.0f, float height = 1.0f, unsigned int segments = 1);
	Model* addWaterPlane(const std::string& name, ShaderPipeline& pipeline, MaterialLibrary& materials, float width = 1.0f, float height = 1.0f, unsigned int segments = 1);
	Model* addCustomModel(std::unique_ptr<Model> model);

	bool removeModel(unsigned int id);
	bool removeModel(const std::string& name);

	Model* getModel(unsigned int id);
	Model* getModel(const std::string& name);
	const std::vector<std::unique_ptr<Model>>& getModels() const { return models; }

	Light* addDirectionalLight(const glm::vec3& dir, const glm::vec3& col, float intensity = 1.0f);
	Light* addPointLight(const glm::vec3& pos, const glm::vec3& col, float intensity = 1.0f);
	Light* addSpotLight(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& col, float innerAngle = 12.5f, float outerAngle = 17.5f, float intensity = 1.0f);
	bool removeLight(size_t index);
	void clearLights();

	std::vector<Light>& getLights() { return lights; }
	const std::vector<Light>& getLights() const { return lights; }

	glm::vec3 getAmbientLight() const { return ambientLight; }
	void setAmbientLight(const glm::vec3& color) { ambientLight = color; }

	void render(Renderer& renderer, const MaterialLibrary& materials, ShaderPipeline& lightingPipeline);
	void collectShadowCasters(std::vector<Mesh*>& out) const;

	SceneStats getStats() const;

};