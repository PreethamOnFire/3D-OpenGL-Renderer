#pragma once
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class ShaderPipeline {
public:
	ShaderPipeline(const char* vertexPath, const char* fragmentPath);
	~ShaderPipeline();
	ShaderPipeline(const ShaderPipeline&) = delete;
	ShaderPipeline& operator=(const ShaderPipeline&) = delete;

	void use() const;
	void bind();
	void applyState() const;
	void restoreState() const;

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setMat4(const std::string& name, const glm::mat4& mat) const;

	bool depthTest = true;
	bool depthWrite = true;
	bool blending = true;
	bool faceCulling = false;
	GLenum cullFace = GL_BACK;
	GLenum blendSrc = GL_SRC_ALPHA;
	GLenum blendDst = GL_ONE_MINUS_SRC_ALPHA;

private:
	unsigned int ID;
};
