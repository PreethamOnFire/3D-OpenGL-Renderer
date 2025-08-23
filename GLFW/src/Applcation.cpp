#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../src/BufferObjects/Mesh.h"
#include "../src/BufferObjects/DefaultObjects/Cube.h"
#include "Shader.h"
#include "Renderer.h"
#include "Scene/Model/Model.h"
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

    void initWindow() {
        window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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
    }

    void mainLoop() {
        Shader shaderProgram("src/VertexShaders/DefaultVertexShader.vs", "src/FragmentShaders/BlinkingShader.fs");
        Shader shaderProgram1("src/VertexShaders/DefaultVertexShader.vs", "src/FragmentShaders/DefaultFragmentShader.fs");


        
        Cube* cube = new Cube(shaderProgram1);
		Model* model = new Model("assets/models/Monkey.obj", shaderProgram1);
		model->setPosition(glm::vec3(0.0f, 0.0f, -5.0f));
		model->setScale(glm::vec3(-0.8f, -0.8f, -0.8f));
        

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer->Clear();

            renderer->GetInput();
            
            renderer->DrawTriangles(cube->cubeMesh);
			model->render(*renderer);

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
        delete cube;
		delete model;
        glfwTerminate();
    }

    void cleanup() {
        delete renderer;
    }
};

int main(void)
{
    Game app = Game();
    app.run();

    return 0;
}