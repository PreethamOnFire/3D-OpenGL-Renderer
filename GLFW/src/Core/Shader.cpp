#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath) : ID(0) {
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

void Shader::use() const {
	glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const {
	GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc != -1) {
		glUniform1i(loc, (int)value);
	}
}

void Shader::setInt(const std::string& name, int value) const {
	GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc != -1) {
		glUniform1i(loc, (int)value);
	}
}

void Shader::setFloat(const std::string& name, float value) const {
	GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc != -1) {
		glUniform1f(loc, value);
	}
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
	GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc != -1) {
		glUniform3fv(loc, 1, &value[0]);
	}
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
	GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc != -1) {
		glUniformMatrix4fv(loc, 1, GL_FALSE, &mat[0][0]);
	}
}

Shader::~Shader() {
	if (ID) {
		glDeleteProgram(ID);
	}
}
