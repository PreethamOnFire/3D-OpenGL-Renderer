#pragma once
#include <glm/glm.hpp>

enum class LightType {
	DIRECTIONAL,
	POINT,
	SPOT
};

class Light {
public:
	LightType type;
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 color;

	float intensity;

	float constant;
	float linear;
	float quadratic;

	//spotlight specific
	float cutOff;
	float outerCutOff;

	// for shadows later on
	bool castsShadows;
	int shadowMapLayer; // layer index into the shared ShadowMapArray this frame; -1 = none assigned (directional/spot)
	glm::mat4 lightSpaceMatrix;

	int shadowCubeLayer;   // light-slot index into the shared CubemapArray this frame; -1 = none assigned (point)
	float shadowFarPlane;  // far-plane distance used for this light's point-shadow depth pass this frame

	Light(LightType lightType = LightType::DIRECTIONAL)
		: type(lightType), position(0.0f), direction(0.0f, -1.0f, 0.0f),
		color(1.0f), intensity(1.0f), constant(1.0f), linear(0.09f),
		quadratic(0.032f), cutOff(glm::cos(glm::radians(12.5f))),
		outerCutOff(glm::cos(glm::radians(17.5f))), castsShadows(false),
		shadowMapLayer(-1), lightSpaceMatrix(1.0f),
		shadowCubeLayer(-1), shadowFarPlane(25.0f) {
	}

	static Light createDirectionalLight(const glm::vec3& dir, const glm::vec3& col, float intensity = 1.0f) {
		Light light(LightType::DIRECTIONAL);
		light.direction = dir;
		light.color = col;
		light.intensity = intensity;
		return light;
	}

	static Light createPointLight(const glm::vec3& pos, const glm::vec3& col, float intensity = 1.0f) {
		Light light(LightType::POINT);
		light.position = pos;
		light.color = col;
		light.intensity = intensity;
		light.constant = 1.0f;
		light.linear = 0.09f;
		light.quadratic = 0.032f;
		return light;
	}

	static Light createSpotLight(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& col, float innerAngle = 12.5f, float outerAngle = 17.5f, float intensity = 1.0f) {
		Light light(LightType::SPOT);
		light.position = pos;
		light.direction = glm::normalize(dir);
		light.color = col;
		light.intensity = intensity;
		light.cutOff = glm::cos(glm::radians(innerAngle));
		light.outerCutOff = glm::cos(glm::radians(outerAngle));
		return light;
	}

};

