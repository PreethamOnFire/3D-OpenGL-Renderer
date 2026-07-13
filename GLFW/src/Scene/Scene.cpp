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

Model* Scene::addCube(const std::string& name, Shader& shader) {
	auto cube = std::make_unique<Model>(name, ModelType::PRIMITIVE_CUBE, shader);
	Model* ptr = cube.get();
	models.push_back(std::move(cube));
	return ptr;
}

Model* Scene::addSphere(const std::string& name, Shader& shader, int segments) {
	auto sphere = std::make_unique<Model>(name, ModelType::PRIMITIVE_SPHERE, shader, 1.0f, 1.0f, segments);
	Model* ptr = sphere.get();
	models.push_back(std::move(sphere));
	return ptr;
}

Model* Scene::addPlane(const std::string& name, Shader& shader, float width, float height, unsigned int segments) {
	auto plane = std::make_unique<Model>(name, ModelType::PRIMITIVE_PLANE, shader, width, height, static_cast<int>(segments));
	Model* ptr = plane.get();
	models.push_back(std::move(plane));
	return ptr;
}

Model* Scene::addWaterPlane(const std::string& name, Shader& shader, float width, float height, unsigned int segments) {
	auto waterPlane = std::make_unique<Model>(name, ModelType::PRIMITIVE_PLANE, shader);
	if (Material* m = waterPlane->getMaterial(0)) {
		m->diffuse = glm::vec3(0.1f, 0.3f, 0.6f);
		m->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	}
	Model* ptr = waterPlane.get();
	models.push_back(std::move(waterPlane));
	return ptr;
}

Model* Scene::addModel(const std::string& name, const std::string& filePath, Shader& shader) {
	auto model = std::make_unique<Model>(name, filePath, shader);
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

void Scene::setSkybox(const std::vector<std::string>& faces, const std::string& directory, Shader& shader) {
	skybox = std::make_unique<SkyBox>(faces, directory, shader);
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

void Scene::updateLightUniforms(Shader& shader, const glm::vec3& viewPos) {
	shader.use();
	shader.setVec3("ambientLight", ambientLight);
	shader.setInt("numLights", static_cast<int>(lights.size()));
	shader.setVec3("viewPos", viewPos);
	for (size_t i = 0; i < lights.size(); i++) {
		std::string base = "lights[" + std::to_string(i) + "]";
		shader.setInt(base + ".type", static_cast<int>(lights[i].type));
		shader.setVec3(base + ".position", lights[i].position);
		shader.setVec3(base + ".direction", lights[i].direction);
		shader.setVec3(base + ".color", lights[i].color);
		shader.setFloat(base + ".intensity", lights[i].intensity);
		shader.setFloat(base + ".constant", lights[i].constant);
		shader.setFloat(base + ".linear", lights[i].linear);
		shader.setFloat(base + ".quadratic", lights[i].quadratic);
		shader.setFloat(base + ".cutOff", lights[i].cutOff);
		shader.setFloat(base + ".outerCutOff", lights[i].outerCutOff);
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

void Scene::render(Renderer& renderer) {
	Camera& cam = renderer.getCamera();
	if (skybox) {
		skybox->render(cam);
	}
	for (const auto& model : models) {
		if (model && model->isLoaded()) {
			if (Shader* shader = model->getShader()) {
				updateLightUniforms(*shader, cam.getEye());
				renderer.bindGlobalUniforms(*shader);
			}
			model->render(renderer);
		}
	}
}


