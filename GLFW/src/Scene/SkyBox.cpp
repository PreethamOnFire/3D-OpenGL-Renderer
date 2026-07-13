#include <GL/glew.h>
#include <vector>
#include <string>
#include <memory>
#include "../Core/Shader.h"
#include "../Rendering/VertexArray.h"
#include "../Rendering/VertexBuffer.h"
#include "../Core/Camera.h"
#include "../Loaders/TextureLoader.h"
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

SkyBox::SkyBox(const std::vector<std::string>& faces, const std::string& directory, Shader& shader) : shader(&shader) {
	VBO = std::make_unique<VertexBuffer>(skyboxVertices, sizeof(skyboxVertices), false);
	VAO = std::make_unique<VertexArray>();
	VAO->addVertexBuffer(*VBO, { 3 });
	textureID = TextureLoader::loadCubeMap(faces, directory);
}

SkyBox::~SkyBox() {
	glDeleteTextures(1, &textureID);
}

void SkyBox::render(Camera& camera) {
    glDepthFunc(GL_LEQUAL);  
    shader->use();
	shader->setInt("skybox", 0);
    glm::mat4 view = glm::mat4(glm::mat3(camera.getViewMatrix()));
    glm::mat4 projection = camera.getProjectionMatrix();
    shader->setMat4("viewMatrix", view);
    shader->setMat4("projectionMatrix", projection);
    VAO->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    VAO->unbind();
    glDepthFunc(GL_LESS); 
}

