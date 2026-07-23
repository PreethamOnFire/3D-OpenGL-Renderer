#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "ShaderPipeline.h"

ShaderPipeline::ShaderPipeline(const char* vertexPath, const char* fragmentPath) : ID(0) {
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);

		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

	}
	catch (const std::ifstream::failure& e) {
		std::cout << "Error: Cannot load in shaders" << std::endl;
		return;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "Error: Vertex Shader failed to compile\n" << infoLog << std::endl;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "Error: Fragment Shader failed to compile\n" << infoLog << std::endl;
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "Error: Shader Program failed to Link\n" << infoLog << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void ShaderPipeline::use() const {
	glUseProgram(ID);
}

void ShaderPipeline::bind() {
	glUseProgram(ID);
	applyState();
}

void ShaderPipeline::applyState() const {
	if (depthTest) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	glDepthMask(depthWrite ? GL_TRUE : GL_FALSE);
	if (blending) {
		glEnable(GL_BLEND);
		glBlendFunc(blendSrc, blendDst);
	} else {
		glDisable(GL_BLEND);
	}
	if (faceCulling) {
		glEnable(GL_CULL_FACE);
		glCullFace(cullFace);
	} else {
		glDisable(GL_CULL_FACE);
	}
}

void ShaderPipeline::restoreState() const {
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
}

void ShaderPipeline::setBool(const std::string& name, bool value) const {
	GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc != -1) {
		glUniform1i(loc, (int)value);
	}
}

void ShaderPipeline::setInt(const std::string& name, int value) const {
	GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc != -1) {
		glUniform1i(loc, (int)value);
	}
}

void ShaderPipeline::setFloat(const std::string& name, float value) const {
	GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc != -1) {
		glUniform1f(loc, value);
	}
}

void ShaderPipeline::setVec3(const std::string& name, const glm::vec3& value) const {
	GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc != -1) {
		glUniform3fv(loc, 1, &value[0]);
	}
}

void ShaderPipeline::setMat4(const std::string& name, const glm::mat4& mat) const {
	GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc != -1) {
		glUniformMatrix4fv(loc, 1, GL_FALSE, &mat[0][0]);
	}
}

ShaderPipeline::~ShaderPipeline() {
	if (ID) {
		glDeleteProgram(ID);
	}
}
