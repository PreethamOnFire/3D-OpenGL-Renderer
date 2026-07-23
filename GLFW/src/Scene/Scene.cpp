#include "Scene.h"
#include <algorithm>
#include <iostream>

Scene::Scene() : ambientLight(0.1f, 0.1f, 0.1f) {
}

Scene::~Scene() {
	clearLights();
	models.clear();
	skybox.reset();
}

Model* Scene::addCube(const std::string& name, ShaderPipeline& pipeline, MaterialLibrary& materials) {
	auto cube = std::make_unique<Model>(name, ModelType::PRIMITIVE_CUBE, pipeline, materials);
	Model* ptr = cube.get();
	models.push_back(std::move(cube));
	return ptr;
}

Model* Scene::addSphere(const std::string& name, ShaderPipeline& pipeline, MaterialLibrary& materials, int segments) {
	auto sphere = std::make_unique<Model>(name, ModelType::PRIMITIVE_SPHERE, pipeline, materials, 1.0f, 1.0f, segments);
	Model* ptr = sphere.get();
	models.push_back(std::move(sphere));
	return ptr;
}

Model* Scene::addPlane(const std::string& name, ShaderPipeline& pipeline, MaterialLibrary& materials, float width, float height, unsigned int segments) {
	auto plane = std::make_unique<Model>(name, ModelType::PRIMITIVE_PLANE, pipeline, materials, width, height, static_cast<int>(segments));
	Model* ptr = plane.get();
	models.push_back(std::move(plane));
	return ptr;
}

Model* Scene::addWaterPlane(const std::string& name, ShaderPipeline& pipeline, MaterialLibrary& materials, float width, float height, unsigned int segments) {
	auto waterPlane = std::make_unique<Model>(name, ModelType::PRIMITIVE_PLANE, pipeline, materials, width, height, static_cast<int>(segments));
	if (Material* m = materials.get(name + "_default")) {
		m->setVec3("diffuse", glm::vec3(0.1f, 0.3f, 0.6f));
		m->setVec3("specular", glm::vec3(1.0f, 1.0f, 1.0f));
	}
	Model* ptr = waterPlane.get();
	models.push_back(std::move(waterPlane));
	return ptr;
}

Model* Scene::addModel(const std::string& name, const std::string& filePath, ShaderPipeline& pipeline, MaterialLibrary& materials) {
	auto model = std::make_unique<Model>(name, filePath, pipeline, materials);
	if (!model->isLoaded()) {
		std::cerr << "Failed to load model: " << filePath << std::endl;
		return nullptr;
	}
	Model* ptr = model.get();
	models.push_back(std::move(model));
	return ptr;
}

Model* Scene::addCustomModel(std::unique_ptr<Model> model) {
	if (!model || !model->isLoaded()) {
		std::cerr << "Invalid or unloaded custom model!" << std::endl;
		return nullptr;
	}
	Model* ptr = model.get();
	models.push_back(std::move(model));
	return ptr;
}

Model* Scene::getModel(unsigned int id) {
	auto it = std::find_if(models.begin(), models.end(),
		[id](const std::unique_ptr<Model>& model) {
			return model->getId() == id;
		});

	return (it != models.end()) ? it->get() : nullptr;
}

Model* Scene::getModel(const std::string& name) {
	auto it = std::find_if(models.begin(), models.end(),
		[name](const std::unique_ptr<Model>& model) {
			return model->getName() == name;
		});
	return (it != models.end()) ? it->get() : nullptr;
}

bool Scene::removeModel(unsigned int id) {
	auto it = std::remove_if(models.begin(), models.end(),
		[id](const std::unique_ptr<Model>& model) {
			return model->getId() == id;
		});
	if (it != models.end()) {
		models.erase(it, models.end());
		return true;
	}
	return false;
}

bool Scene::removeModel(const std::string& name) {
	auto it = std::remove_if(models.begin(), models.end(),
		[name](const std::unique_ptr<Model>& model) {
			return model->getName() == name;
		});
	if (it != models.end()) {
		models.erase(it, models.end());
		return true;
	}
	return false;
}

void Scene::setSkybox(const std::vector<std::string>& faces, const std::string& directory, ShaderPipeline& pipeline) {
	skybox = std::make_unique<SkyBox>(faces, directory, pipeline);
}

void Scene::removeSkybox() {
	skybox.reset();
}

bool Scene::hasSkybox() const {
	return skybox != nullptr;
}

Light* Scene::addDirectionalLight(const glm::vec3& dir, const glm::vec3& col, float intensity) {
	lights.push_back(Light::createDirectionalLight(dir, col, intensity));
	return &lights.back();
}

Light* Scene::addPointLight(const glm::vec3& pos, const glm::vec3& col, float intensity) {
	lights.push_back(Light::createPointLight(pos, col, intensity));
	return &lights.back();
}

Light* Scene::addSpotLight(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& col, float innerAngle, float outerAngle, float intensity) {
	lights.push_back(Light::createSpotLight(pos, dir, col, innerAngle, outerAngle, intensity));
	return &lights.back();
}

void Scene::updateLightUniforms(ShaderPipeline& pipeline, const glm::vec3& viewPos) {
	pipeline.use();
	pipeline.setVec3("ambientLight", ambientLight);
	pipeline.setInt("numLights", static_cast<int>(lights.size()));
	pipeline.setVec3("viewPos", viewPos);
	for (size_t i = 0; i < lights.size(); i++) {
		std::string base = "lights[" + std::to_string(i) + "]";
		pipeline.setInt(base + ".type", static_cast<int>(lights[i].type));
		pipeline.setVec3(base + ".position", lights[i].position);
		pipeline.setVec3(base + ".direction", lights[i].direction);
		pipeline.setVec3(base + ".color", lights[i].color);
		pipeline.setFloat(base + ".intensity", lights[i].intensity);
		pipeline.setFloat(base + ".constant", lights[i].constant);
		pipeline.setFloat(base + ".linear", lights[i].linear);
		pipeline.setFloat(base + ".quadratic", lights[i].quadratic);
		pipeline.setFloat(base + ".cutOff", lights[i].cutOff);
		pipeline.setFloat(base + ".outerCutOff", lights[i].outerCutOff);
	}
}

bool Scene::removeLight(size_t index) {
	if (index >= lights.size()) {
		return false;
	}
	lights.erase(lights.begin() + index);
	return true;
}

void Scene::clearLights() {
	lights.clear();
}

void Scene::render(Renderer& renderer, const MaterialLibrary& materials, ShaderPipeline& lightingPipeline) {
	Camera& cam = renderer.getCamera();
	if (skybox) {
		skybox->render(cam);
	}
	updateLightUniforms(lightingPipeline, cam.getEye());
	renderer.bindGlobalUniforms(lightingPipeline);
	for (const auto& model : models) {
		if (model && model->isLoaded()) {
			model->render(renderer, materials);
		}
	}
}


