#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../src/BufferObjects/Mesh.h"
#include "Shader.h"
#include "Renderer.h"
#include "Scene/Model/Model.h"
#include "Scene/Scene.h"
#include <vector>

class Game {
public:
    void run() {
        initOpenGL();
        gameSetUp();
        mainLoop();
        cleanup();
    }
private:
    GLFWwindow* window;
    Renderer* renderer;
	Scene* scene;
	Shader* skyBoxShader;
    Shader* ObjectShader;
	Shader* DebugShader;

    void initWindow() {
        window = glfwCreateWindow(1920, 1080, "Hello World", NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            return;
        }
    }

    void initOpenGL() {
        if (!glfwInit())
            return;
        initWindow();
        glfwMakeContextCurrent(window);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
        if (glewInit() != GLEW_OK)
            std::cout << "Issue with GLEW" << std::endl;

        std::cout << glGetString(GL_VERSION) << std::endl;
    }

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
        Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
        game->handleWindowResize(width, height);
    }

    void handleWindowResize(int width, int height) {
        glViewport(0, 0, width, height);

        if (renderer && renderer->camera) {
            renderer->camera->updateProjectionMatrix(width, height);
        }
    }

    void gameSetUp() {
        renderer = new Renderer(*window);
		scene = new Scene();
        skyBoxShader = new Shader("src/VertexShaders/SkyBoxVertexShader.vs", "src/FragmentShaders/SkyBoxFragmentShader.fs");
		ObjectShader = new Shader("src/VertexShaders/DefaultVertexShader.vs", "src/FragmentShaders/TextureShader.fs");
        std::vector<std::string> faces
        {
            "right.jpg",
            "left.jpg",
            "top.jpg",
            "bottom.jpg",
            "front.jpg",
            "back.jpg"
		};
        std::string skyboxDir = "assets/skybox";
        scene->setSkybox(faces, skyboxDir, *skyBoxShader);
		Model* monkey = scene->addModel("Monkey", "assets/models/Monkey.obj", *ObjectShader);
        monkey->setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
        monkey->setScale(glm::vec3(0.8f, -0.8f, 0.8f));
		Model* normandy = scene->addModel("Normandy", "assets/models/Normandy/Normandy.obj", *ObjectShader);
        normandy->setPosition(glm::vec3(0.0f, -1.0f, -5.0f));
        normandy->setScale(glm::vec3(0.001f, -0.001f, 0.001f));

        scene->addPointLight(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        scene->addDirectionalLight(
            glm::vec3(0.3f, 1.0f, 0.5f),
            glm::vec3(1.0f, 0.95f, 0.8f),
            1.0f
        );
    }

    void mainLoop() {
        
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer->Clear();

            renderer->GetInput();
            
			scene->render(*renderer);

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    void cleanup() {
        delete renderer;
        delete scene;
		delete skyBoxShader;
        delete ObjectShader;
        glfwDestroyWindow(window);
		glfwTerminate();
    }
};

int main(void)
{
    Game app = Game();
    app.run();

    return 0;
}