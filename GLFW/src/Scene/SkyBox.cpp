#pragma once
#include <GL/glew.h>
#include <vector>
#include <string>
#include "../Shader.h"
#include "../BufferObjects/VertexArray.h"
#include "../BufferObjects/VertexBuffer.h"
#include "../Camera.h"
#include "../Utility/TextureLoader.h"
#include "SkyBox.h"

const float SkyBox::skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

SkyBox::SkyBox(const std::vector<std::string>& faces, std::string& directory, Shader& shader) : shader(&shader) {
	VBO = new VertexBuffer(skyboxVertices, sizeof(skyboxVertices), false);
	VAO = new VertexArray();
	VAO->addVertexBuffer(*VBO, { 3 });
	textureID = TextureLoader::loadCubeMap(faces, directory);
}

SkyBox::~SkyBox() {
    delete VBO;
    delete VAO;
	glDeleteTextures(1, &textureID);
}

void SkyBox::render(Camera& camera) {
    glDepthFunc(GL_LEQUAL);  
    shader->use();
    glm::mat4 view = glm::mat4(glm::mat3(camera.viewMatrix)); 
    glm::mat4 projection = camera.projectionMatrix;
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    VAO->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    VAO->unBind();
    glDepthFunc(GL_LESS); 
}

